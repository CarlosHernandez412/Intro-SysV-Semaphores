/* An example of a union in C
 */

#include <stdio.h>

int main() {
   union test {
      int num1;
      float num2;
   } my_test;

   printf("Address of variables:\n");
   printf("int:   %p \n", &my_test.num1);
   printf("float: %p \n", &my_test.num2);

   my_test.num1 = 10;
   printf("int: %d  float: %f\n", my_test.num1, my_test.num2);

   my_test.num2 = 5.5;
   printf("int: %d  float: %f\n", my_test.num1, my_test.num2);

   return 0;
}
