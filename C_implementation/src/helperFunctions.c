#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "helperFunctions.h"
#include "arithmetic.h"
#include "postgres.h"

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

void printRangeSet(Int4RangeSet a){
  printf("{");
  for (size_t i=0; i<a.count; i++){
    printf("[%d, %d]", a.ranges[i].lower, a.ranges[i].upper);
  }
  printf("}\n");
}

bool validRange(Int4Range a){
  return a.lower < a.upper;
}

Int4Range lift(int x){
  Int4Range rv = {x, x+1};
  return rv;
}

// https://www.geeksforgeeks.org/c/comparator-function-of-qsort-in-c/#
static int compare_ranges(const void* range1, const void* range2){
  const Int4Range r1 = *(Int4Range*)range1;
  const Int4Range r2 = *(Int4Range*)range2;




}


// double pass. Order by lower bound first, and then upper bound. can maybe do in 1 pass
Int4RangeSet sort(Int4RangeSet vals){
  Int4RangeSet sorted;
  sorted.count = vals.count;
  sorted.ranges = palloc(sizeof(Int4Range) * vals.count);
  memcpy(sorted.ranges, vals.ranges, sizeof(Int4Range) * vals.count);
  
  int n = sizeof(vals) / sizeof(Int4Range);
  qsort(vals, vals.count, n, compare_ranges);

  // // Int4RangeSet sorted2;
  // sorted1.ranges = palloc(sizeof(Int4Range) * vals.count);
  // sorted1.count = 0;
  // // sorted2.ranges = palloc(sizeof(Int4Range) * vals.count);

  // for (size_t i=0; i<vals.count; i++){
  //   Int4Range curr = vals.ranges[i];
  //   // auto append first element
  //   if (i == 0){
  //     sorted1.ranges[0] = curr;
  //     sorted1.count += 1;
  //   }
  //   else{
  //     for (size_t j=0; j<sorted1.count; j++){
  //       // check on lower bound
  //       if (sorted1.ranges[j].lower > vals.ranges[i].lower){

  //       }
  //       else if (sorted1.ranges[j].lower == vals.ranges[i].lower){
          
  //       }
  //     }
  //   }
    


  // }
  

}

// Int4RangeSet normalize(Int4RangeSet vals){
//   Int4RangeSet 
// }
