#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "helperFunctions.h" 

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
void printRange(Int4Range a){
  if (a.isNull){
    printf("NULL\n");
  }
  else{
    printf("[");
    printf("%d, %d", a.lower, a.upper);
    printf(")\n");
  }
}
void printRangeSet(Int4RangeSet a){
  printf("{");
  
  for (size_t i=0; i<a.count; i++){
    if (a.ranges[i].isNull) {
      printf(" NULL ");
    }
    else {
      printf(" [%d, %d)", a.ranges[i].lower, a.ranges[i].upper);
    }
  }
  printf("}\n");
}

Int4RangeSet filterOutNulls(Int4RangeSet vals) {
  if (!vals.containsNull) {
    return vals;
  }
  
  Int4RangeSet filteredVals;
  size_t nonNullCount;
  size_t i;
  
  nonNullCount = 0;
  for (i = 0; i < vals.count; i++) {
    if (!vals.ranges[i].isNull) nonNullCount++;
  }

  filteredVals.count = nonNullCount;
  filteredVals.ranges = malloc(sizeof(Int4Range) * filteredVals.count);
  filteredVals.containsNull = false;
  // filteredVals.containsNull = vals.containsNull;

  int idx;
  idx = 0;
  for (i = 0; i < vals.count; i++) {
    if (!vals.ranges[i].isNull) {
      filteredVals.ranges[idx].isNull = false;
      filteredVals.ranges[idx++] = vals.ranges[i];
    }
  }

  return filteredVals;
}


int main(){
  Int4Range a = {1,3};
  Int4Range b = {10,21};
  Int4Range c = {5,8};
  Int4Range d = {7,12};
  Int4Range e = {0,0};
  Int4Range f = {6,11};

  Int4Range n;
  n.isNull = true;


  Multiplicity mult1 = {0,1};
  Multiplicity mult2 = {1,2};
  Multiplicity mult3 = {0,2};

  Int4Range a_ranges[] = {a, n, f};
  Int4Range b_ranges[] = {b, n};
  Int4RangeSet s1 = {a_ranges, 3, true};
  Int4RangeSet s2 = {b_ranges, 2, true};
  
  // Int4RangeSet rv1 = range_set_add(s1, s2); 
  // printRangeSet(rv1);

  Int4RangeSet rv2 = filterOutNulls(s1);
  printRangeSet(rv2);

  // Int4Range rv1 = floatIntervalSetMult(s1, mult1);  
  // printRange(rv1);
  // Int4Range rv2 = floatIntervalSetMult(s1, mult2);  
  // printRange(rv2);
  // Int4Range rv3 = floatIntervalSetMult(s1, mult3);  
  // printRange(rv3);


  // Int4Range c_ranges[] = {f, a, e, c};
  // Int4RangeSet s3 = {c_ranges, 4};
  // Int4RangeSet tn_rv = testNormalize(s3);
  // printRangeSet(tn_rv);


  return 0;
}

