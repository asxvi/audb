#include "arithmetic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


// can also implement macro or add Utility.h/.c
// https://www.delftstack.com/howto/c/c-max-and-min-function/
int MIN(int My_array[], int len) {
  int num = My_array[0];
  for (int i = 1; i < len; i++) {
    if (My_array[i] < num) {
      num = My_array[i];
    }
  }
  return num;
}
int MAX(int My_array[], int len) {
  int num = My_array[0];
  for (int i = 1; i < len; i++) {
    if (My_array[i] > num) {
      num = My_array[i];
    }
  }
  return num;
}

bool range_lt(Int4RangeSet set1, Int4RangeSet set2){

  
  return true;
}


int main(){

  return 0;
}