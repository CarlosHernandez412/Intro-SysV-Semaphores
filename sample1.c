/* sample1.c
 * demonstrates how to create threads 
 * also demonstrates how threads are independently scheduled
 * 
 * $ gcc -o sample1 sample1.c -lpthread
 * $ ./sample1
*/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>  
#include <stdlib.h>

#define NUM_THREADS 5 

void *printIt(void *thread_num) {

  /* do the assigment into a long since pointers are 8 bytes */
  long tnum = (long)thread_num;

  /* demonstrate process wide syscall - sleep() applies to the entire
   * process, so all threads will sleep when the thread 0 calls this */
  if (tnum == 0) sleep(6);

  printf("hello there from thread #%ld!\n",tnum);
  pthread_exit(NULL);
} 

int main( int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;

   /* Create an array of threads */
   for (t=0; t<NUM_THREADS; t++) {
      printf("creating thread %ld\n", t);

      /* pthread_create stuffs thread id into threads array */
      rc = pthread_create( &threads[t], NULL, printIt, (void *)t);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n",rc);
         exit(-1);
      }
   }

   /* Block parent thread until each child thread exits */
   for (t=0; t<NUM_THREADS; t++) {
      if (pthread_join(threads[t], NULL) < 0) {
          perror("pthread_join: ");
      }
      printf("joined thread %ld\n", t);
   } 

   /* parent can now exit its own thread, which is functionally equivalent
      to calling exit() since the parent thread is the only thread now */
   pthread_exit(NULL);
}


