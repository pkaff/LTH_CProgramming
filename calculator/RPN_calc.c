#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define MAXSIZE (15)

//function declarations
int is_operator(char c);
void push(int nbr);
void do_operator(char c);
int pop();
int concat(int x, int y);

//global variable declarations (undviks i nästkommande labbar.)
int stack_end = 0;
int stack[MAXSIZE];

int main(int argc, char** argv) {
  int c; //returtypen av getchar är ju ologiskt nog en int (ascii-koden), char kan vara för litet för att lagra hela läsningen.
  int digit;
  int result;
  int next_char;
  
  //get char until EOF (ctrl+D)
  printf("Input RPN-formatted line: ");

  do {
    c = getchar();

    //If space, skip
    if (c == ' ') {
      continue;
    }

    //If newline, pop
    if (c == '\n') {
      result = pop();
      printf("The result is: %d \n", result);
      continue;
    }

    //if integer, push it
    if (isdigit(c)) {
      digit = c -'0';
      while (c != ' ') {
	next_char = getchar();
	if (next_char == ' ') {
	  push(digit);
	  break;
	}
	if (!isdigit(next_char)) {
	  fprintf(stderr, "Syntax error!");
	  exit(1);
	}
	digit = concat(digit, next_char-'0');
	c = next_char;
      }
      continue;
    }
    
    //if operator, do corresponding operation
    if (is_operator(c)) {
      do_operator(c);
      continue;
    }
    
    //if EOF
    if (c == EOF) {
      break;
    }
    
    //if not any of the above, invalid input
    fprintf(stderr, "Invalid input argument! \n");
    exit(1);
  } while (c != EOF);
  return 0;
}

int concat(int x, int y) {
  return x*10 + y;
}

void push(int nbr) {
  if (stack_end >= MAXSIZE) {
    fprintf(stderr, "Stack is full, terminating! \n");
    exit(1);
  }
  stack[stack_end++] = nbr;  
  //printf("pushed: %d\n", stack[stack_end - 1]);
}

int is_operator(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/';
}

void do_operator(char c) {
  int e1;
  int e2;
  int res;

  e1 = pop();
  e2 = pop();

  switch (c) {
  case '+':
    res = e2 + e1;
    push(res);
    return;
  case '-':
    res = e2 - e1;
    push(res);
    return;
  case '*':
    res = e2 * e1;
    push(res);
    return;
  case '/':
    if (e1 == 0) {
      fprintf(stderr, "Division by 0! \n");
      exit(1);
    }
    res = e2 / e1;
    push(res);
    return;
  default:
    fprintf(stderr, "Invalid input argument! \n");
    return;
  }
}

int pop() {
  int temp;
  if (stack_end == 0) {
    fprintf(stderr, "Stack empty, can't pop! \n");
    exit(1);
  }
  //printf("popped: %d\n", stack[stack_end-1]);
  temp = stack[--stack_end];
  return temp;
}

