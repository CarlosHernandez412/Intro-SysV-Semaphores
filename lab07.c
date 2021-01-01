#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      /* header file for the POSIX API */
#include <string.h>      /* string handling functions */
#include <errno.h>       /* for perror() call */
#include <pthread.h>     /* POSIX threads */ 
#include <sys/ipc.h>     /* SysV IPC header */
#include <sys/sem.h>     /* SysV semaphore header */
#include <stdint.h>      /* For 64 bit integers */
/* lab06.c   Basic multithreading with POSIX threads
 *
 * Carlos Hernandez  22 Oct 2020 
 */


/* Structure to pass information to thread function */



typedef struct {
    int myid;    /* 'i' value from parent thread */
    int start;   /* starting index in array */
    int end;     /* ending index in array */
} t_info;

/* Macros for the program */
#define MAX_SIZE  100000  /* Maximum number of integers in array */
#define MAX_NAME     256  /* Most filesystems only support 255-char filenames */
#define NUM_THREADS    6  /* Number of child threads to spawn */

/* Global variables for the array */
int array[MAX_SIZE];
int count;
int64_t globalSum = 0;
int semid;
int ret;

struct sembuf grab[2];    
struct sembuf release[1];

union semun {
  int  val; 
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *_buf;
} my_semun; 
int val=0;



void *doubleIt(void *);
   
	
int main(int argc, char *argv[]) {
    int i, ret, slice;
	int nsem = 1;
    char filein[MAX_NAME];
    FILE *infile;
    pthread_t threads[NUM_THREADS];
    t_info data[NUM_THREADS];
	my_semun.val = 0;

	char pathname[128];
   getcwd(pathname,200);
   strcat(pathname,"/foo");       /* foo must exist in your directory */
  key_t ipckey = ftok(pathname, 21);
  semid = semget(ipckey, nsem, 0666 | IPC_CREAT);
  semctl(semid, 0, SETVAL, my_semun);
    /* Check if filename argument was given */
    if(argc < 2){
        /* argv[0] contains name of executable from command-line */
        printf("Usage: %s <input_file> \n", argv[0]); 
        exit(1);
    }

    count = 0;  /* Tracks actual number of elements in array */
    strncpy(filein, argv[1], MAX_NAME);
   

    /* Open the file stream for read-only */
    infile = fopen(filein, "r");
    if(infile == NULL) {
        perror("fopen: ");
        exit(EXIT_FAILURE);
    }


    /* Read the integers in from the file and put in the array */
    do {
        ret = fscanf(infile, "%d", &i);
        if(ret > 0) array[count++] = i; /* Got a number! */
    } while(ret != EOF && count < MAX_SIZE);

    printf("Read in %d integers from file %s.\n", count, filein);
    fclose(infile); /* close the file stream */
 
    
	/*if (semid < 0) {
		perror ("semget: ");
		exit (1);
	}
	*/
	
	
   grab[0].sem_num = 0;        /* Associate this with first (only) semaphore */
   grab[0].sem_flg = SEM_UNDO; /* Release semaphore if premature exit */
   grab[0].sem_op =  0;        /* 1st action: wait until sem value is zero */

   grab[1].sem_num = 0;        /* Also associated with first semaphore */
   grab[1].sem_flg = SEM_UNDO; /* Release semaphore if premature exit */
   grab[1].sem_op = +1;        /* 2nd action: increment semaphore value by 1 */

   release[0].sem_num = 0;     /* Also associated with first semaphore */
   release[0].sem_flg = SEM_UNDO; /* Release semaphore if premature exit */
   release[0].sem_op = -1;     /* 1st action: decrement semaphore value by 1 */

    /* Use integer division to determine how many elements to give to each
     * child thread. The last thread will get the remaining elements */
    slice = count / NUM_THREADS; 
    printf("Passing %d elements to each child thread.\n", slice);

    for(i = 0; i < NUM_THREADS; i++) {
       data[i].myid = i;
        data[i].start = (i * slice);
        if(i == (NUM_THREADS - 1)) // last child 
            data[i].end = count;
        else
            data[i].end = data[i].start + slice;

        printf("Calling pthread_create for thread %d...\n", i);
        ret = pthread_create(&threads[i], NULL, doubleIt, (void*) &(data[i]) );
        if(ret) {
            perror("pthread_create: ");
            exit(EXIT_FAILURE);
        }
    }

    for(i = 0; i < NUM_THREADS; i++) {
        printf("Calling pthread_join for thread %d...\n", i);
        if(pthread_join(threads[i], NULL) < 0) {
            perror("pthread_join: ");
        }
    }
	printf ("Overall sum is: %ld.\n ", globalSum);
	ret = semctl(semid, 0, IPC_RMID);

    exit(EXIT_SUCCESS);
}

void *doubleIt(void *info) {
   int64_t LocalSum=0;
   int i;
    t_info *data;\
	

    data = (t_info *)info;
	
	//
	//
	//


    printf("Thread data: id=%d start=%d end=%d\n", 
            data->myid, data->start, data->end );

    for(i = data->start; i < data->end; i++)
       LocalSum += array[i];
   
	printf("Thread %d: Local sum is %ld \n", data->myid, LocalSum);
	printf("Thread %d requesting semaphore access...\n",data->myid);
	ret=semop(semid, grab, 2);
	printf ("Thread %d updating sum.\n",data->myid);
	globalSum += LocalSum;
	
	printf("Thread %d releasing semaphore.\n", data->myid);
	ret=semop(semid, release, 1);
	
    printf("Thread %d done with task.\n", data->myid);

    pthread_exit((void*)EXIT_SUCCESS);
}