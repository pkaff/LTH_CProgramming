#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ctype.h>

#define MAX_NO_DIGITS (158)

int main(int argc, char** argv) {
  int n; //Input
  int base = 10; //Base 10
  int j; //Loop variable
  int carry, d; //Multiplication assistants
  int last, i; // Indices to the big number's digits
  int factorial[MAX_NO_DIGITS]; //Vector, 100! has 158 digits
  int nbr_corr_outputs;

  //nbr_corr_outputs 1 if integer, 0 if something else
  printf("Enter integer between 1 and 100 to calculate factorial: ");
  nbr_corr_outputs = scanf("%d", &n);

  //Error if n > 100, n = 0 or n is not an integer
  if (n > 100 || n == 0 || nbr_corr_outputs != 1) {
    fprintf(stderr, "Enter an integer between 1 and 100!\n");
    exit(1);
  }

  //Error if n < 0
  if (n < 0) {
    fprintf(stderr, "Factorial of negative number not defined!\n");
    exit(1);
  }
  
  factorial[0] = 1; //Initialize factorial's smallest digit to 1
  last = 1; //Index for which position in the vector we're in

  //Outer loop, calculates j, 1*j, (1*2)*j, ... (1*2*3*...)n
  for (j = 1; j <= n; j++) {
    //Reset carry
    carry = 0;
    //Inner loop, last is depending on the number of digits in our current nbr
    //i.e. the result
    for (i = 0; i < last; i++) {
      //Loop through all previous results (factorial[i]) and multiply with
      //outer loop variable, j. Add carry if any.
      d = factorial[i]*j + carry;
      //Take smallest digit and place it in correct place in vector
      factorial[i] = d % base;
      //Save carry
      carry = d / base;
    }
    //Handle carry
    while (carry > 0) {
      //Increase index for biggest number
      last = last + 1;
      //Take smallest digit of carry and place in vector
      factorial[last-1] = carry % base;
      //Repeat if carry > base (10)
      carry = carry / base;
    }
  }

  //Print reversed order, biggest number is in the highest index
  printf("%d! = ", n);
  for (i = last - 1; i >= 0; i--) {
    printf("%d", factorial[i]);
  }
  printf("\n");

  return 0;
}
