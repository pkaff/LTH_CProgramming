#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "poly.h"

struct poly_t {
  poly_elem *first;
};

struct poly_elem {
  poly_elem *next;
  int koeff;
  long exp;
};

void add_elem(poly_t* poly, poly_elem* element);
int concat(int x, int y);
void* xmalloc(size_t size);

poly_t* new_poly_from_string(const char* string) {
  int i, k;
  int sign_index, koeff, last_x_index, string_length;
  long exp;
 
  //Declare element
  poly_elem* element;

  //Allocate 1 polynom
  poly_t *polynom;
  polynom = xmalloc(sizeof(struct poly_t));
  polynom->first = NULL;

  sign_index = -2;
  koeff = 1;
  exp = 0;
  last_x_index = 0;
  string_length = strlen(string);

  //WE ARE ASSUMING THAT POLYNOMIALS ARE WRITTEN IN DEGREEORDER WITH HIGHEST DEGREE FIRST (which makes sense, since polynomials often are written that way)
  //Loop through the string
  for (i = 1; i < string_length; i++) {
    //when we find x
    if (string[i - 1] == 'x') {
      //last x index is here
      last_x_index = i - 1;

      //Allocate 1 new element
      element = xmalloc(sizeof(struct poly_elem));
      element->koeff = 1;
      element->exp = 1;
      element->next = NULL;

      //if the sign was '-'
      if (sign_index >= 0) {
	if (string[sign_index] == '-') {
	  element->koeff = -1;
	} else { //if sign was '+'
	  element->koeff = 1;
	}
      }

      //Unless sign was right in front of x (x is at i - 1, space at i - 2)
      if (sign_index != i - 3) {
	//take first number of the koeff
	koeff = string[sign_index + 2] - '0';
	//collect rest of the numbers in the koefficient
	for (k = sign_index + 2; k < last_x_index - 1; k++) {
	  koeff = concat(koeff, string[k + 1] - '0');
	}
	element->koeff = element->koeff * koeff;
      }
      
      k = last_x_index;
      //find index of next sign
      while (string[k] != '+' && string[k] != '-' && k < string_length - 1) {
	k++;
      }
      sign_index = k;

      //if we have exponent of x
      if (string[last_x_index + 1] == '^') {
	//save first exp
	exp = string[last_x_index + 2] - '0';

	//loop until next sign (stop at sign_index - 3 since space separates last exponent number and sign)
	for (k = last_x_index + 2; k < sign_index - 2; k++) {
	  exp = concat(exp, string[k + 1] - '0');
	}
	element->exp = exp;
      } else {
	exp = 1;
      }

      //add the element
      add_elem(polynom, element);
      
    } //end if we find x

  } //end loop through string

  //if constant exist at end
  if (last_x_index < sign_index) {

    //Allocate 1 new element
    element = malloc(sizeof(struct poly_elem));
    element->koeff = 1;
    element->exp = 0;
    element->next = NULL;

    //handle constant
    if (string[sign_index] == '-') {
      element->koeff = -1;
    } else {
      element->koeff = 1;
    }

    //take first number of the koeff
    koeff = string[sign_index + 2] - '0';
    //collect rest of the numbers in the koefficient
    for (k = sign_index + 2; k < string_length - 1; k++) {
      koeff = concat(koeff, string[k + 1] - '0');
    }
    element->koeff = element->koeff * koeff;

    //add the element
    add_elem(polynom, element);

  } //end if constant exist
  return polynom;
}

void free_poly(poly_t* p) {
  poly_t* header;
  poly_elem* node;
  poly_elem* temp;

  node = p->first;
  header = p;

  while (node != NULL) {
    temp = node;
    node = node->next;
    free(temp);
  }
  
  free(header);
}

poly_t* mul(poly_t* p, poly_t* q) {
  poly_elem* e_p = p->first;
  poly_elem* e_q = q->first;
  poly_t* res;
  poly_elem* temp;

  //Error if no elements
  if (p->first == NULL || q->first == NULL) {
    fprintf(stderr, "One or more polynomials are NULL");
    exit(1);
  }

  //Allocate memory for res and temp
  res = xmalloc(sizeof(struct poly_t));
  res->first = NULL;
  temp = xmalloc(sizeof(struct poly_elem));
  temp->koeff = 1;
  temp->exp = 0;
  temp->next = NULL;

  //Loop through all elements in the polynomials, size order is solved by itself but we need to consider
  //when we get 2 elements of the same degree; add them.
  while (e_p != NULL) {
    while(e_q != NULL) {
      temp->koeff = e_p->koeff * e_q->koeff;
      temp->exp = e_p->exp + e_q->exp;
      e_q = e_q->next;

      //Add temp to res
      add_elem(res, temp); 

      //Allocate another element
      temp = xmalloc(sizeof(struct poly_elem));
      temp->koeff = 1;
      temp->exp = 0;
      temp->next = NULL;
    }
    e_q = q->first;
    e_p = e_p->next;
  }

  free(temp);

  return res;
}

void print_poly(poly_t* p) {
  poly_elem* temp;
  int sign;

  temp = p->first;

  //Nothing to print
  if (temp == NULL) {
    return;
  }

  //Print first element
  if (temp->koeff != 1) {
    if (temp->koeff > 0) {
      if (temp->exp != 1) {
	printf("%d x^%ld", temp->koeff, temp->exp);
      } else {
	printf("%d x", temp->koeff);
      }
    } else {
      if (temp->exp != 1) {
	printf("-%d x^%ld", abs(temp->koeff), temp->exp);
      } else {
	printf("-%d x", abs(temp->koeff));
      }
    }
  } else if (temp->exp != 1) {
    printf("x^%ld", temp->exp);
  } else {
    printf("x ");
  }

  temp = temp->next;

  while (temp != NULL) {
    //Check sign
    if (temp->koeff < 0) {
      sign = -1;
    } else {
      sign = 1;
    }

    //If constant
    if (temp->exp == 0) {
      if (sign == -1) {
  	printf(" - %d", abs(temp->koeff));
      } else {
  	printf(" + %d", temp->koeff);
      }
    }
    
    //if x^1
    if (temp->koeff != 1 && temp->exp == 1) {
      if (sign == -1) {
	printf(" - %d x", abs(temp->koeff));
      } else {
	printf(" + %d x", temp->koeff);
      }
    } else if (temp->koeff == 1 && temp->exp == 1) {
      if (sign == -1) {
	printf(" - x");
      } else {
	printf(" + x");
      }
    }

    //other case
    if (temp->koeff != 1 && temp->exp > 1) {
      if (sign == -1) {
	printf(" - %d x^%ld", abs(temp->koeff), temp->exp);
      } else {
	printf(" + %d x^%ld", temp->koeff, temp->exp);
      }
    } else if (temp->koeff == 1 && temp->exp > 1) {
      if (sign == -1) {
	printf(" - x^%ld", temp->exp);
      } else {
	printf(" + x^%ld", temp->exp);
      }
    }
    temp = temp->next;
  } //end while
  printf("\n");
}

//Adds elements in size order (exponents)
void add_elem(poly_t* poly, poly_elem* element) {
  poly_elem* temp;
  poly_elem* pre_temp;

  //if empty list
  if (poly->first == NULL) {
    poly->first = element;
    return;
  }

  temp = poly->first;
  pre_temp = temp;

  //if more than 1 element, add element to list
  while (temp != NULL) {
    //same exp, add koeffs and free
    if (temp->exp == element->exp) {
      temp->koeff = temp->koeff + element->koeff;
      free(element);
      return;
    } else if (temp->exp < element->exp) {
      //if we're at first element in the list, put element first
      if (poly->first == temp) {
	poly->first = element;
	element->next = temp;
	return;
      }
      //put element before temp
      element->next = temp;
      pre_temp->next = element;      
      return;
    } else if (temp->exp > element->exp) {
      //check with next element
      if (temp->next == NULL) {
	temp->next = element;
	return;
      }
      pre_temp = temp;
      temp = temp->next;
    }
  } //end while
}

void* xmalloc(size_t size) {
  void* mem;
  mem = malloc(size);
  if (mem == NULL) {
    fprintf(stderr, "Out of memory!");
    exit(1);
  }
  return mem;
}

int concat(int x, int y) {
  return x*10 + y;
}
