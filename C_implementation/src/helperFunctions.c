#include <string.h>
#include "helperFunctions.h"

// #include "postgres.h" /////

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

  if(r1.lower != r2.lower){
    return r1.lower < r2.lower ? -1 : 1;
  }
  return r1.upper < r2.upper ? -1 : 1;

}

// using C quicksort to sort on lower, upper
Int4RangeSet sort(Int4RangeSet vals){
  Int4RangeSet sorted;
  
  if (vals.count == 0){
    sorted.count = 0;
    sorted.ranges = NULL;
    return sorted;
  }

  sorted.count = vals.count;
  sorted.ranges = malloc(sizeof(Int4Range) * vals.count);
  memcpy(sorted.ranges, vals.ranges, sizeof(Int4Range) * vals.count);
  
  // int n = sizeof(vals) / sizeof(Int4Range);
  qsort(sorted.ranges, vals.count, sizeof(Int4Range), compare_ranges);
  return sorted;
}

Int4RangeSet normalize(Int4RangeSet vals){
  Int4RangeSet normalized;

  if (vals.count == 0){
    normalized.count = 0;
    normalized.ranges = NULL;
    return normalized;
  }
  
  Int4RangeSet sorted = sort(vals);

  normalized.count = 0;
  normalized.ranges = malloc(sizeof(Int4Range) * sorted.count);
  
  Int4Range prev = sorted.ranges[0];

  for(size_t i=1; i<sorted.count; i++){
    Int4Range curr = sorted.ranges[i];
    if (overlap(prev, curr)){
      prev.lower = (curr.lower < prev.lower) ? curr.lower : prev.lower;
      prev.upper = (curr.upper > prev.upper) ? curr.upper : prev.upper;
    }
    // no overlap, so add entire range
    else{
      normalized.ranges[normalized.count++] = prev;
      prev = curr;
    }
  }
  
  // account for last range
  normalized.ranges[normalized.count++] = prev;
  
  return normalized;
}

bool overlap(Int4Range a, Int4Range b){
  return (a.upper-1 >= b.lower);
}

bool contains(Int4Range a, Int4Range b){
  return (a.lower <= b.lower && b.lower <= a.upper 
      && a.lower <= b.upper && b.upper <= a.upper);
}

// confusion example: a(1,5) b(2,9)
int range_distance(Int4Range a, Int4Range b){
  if(contains(a, b) || contains(b, a)){
    return 0;
  }
  else if((a.upper-1) < b.lower){
    return (b.lower - (a.upper-1));
  }
  else {
    return (a.lower - (b.upper-1));
  }
}

int range_distance2(Int4Range a, Int4Range b){
  if(contains(a, b) || contains(b, a)){
    return 0;
  }
  else if((a.upper) < b.lower){
    return (b.lower - (a.upper));
  }
  else {
    return (a.lower - (b.upper));
  }
}

Int4RangeSet reduceSize(Int4RangeSet vals, int numRangesKeep){
  Int4RangeSet normalized;

  if (vals.count == 0){
    normalized.count = 0;
    normalized.ranges = NULL;
    return normalized;
  }
  else if (vals.count <= numRangesKeep){
    return vals;
  }
  

  Int4RangeSet sortedInput = sort(vals);
  Int4Range prev;
  Int4Range curr;
  int distance = -1;
  int currIndex = 0;
  int prevIndex = 0;
  int currNumRanges = sortedInput.count;

  while(currNumRanges > numRangesKeep){
    prev = sortedInput.ranges[0];

    int bestDist = -1;
    int  = -1;

    // greedy look for smallest remaining gap
    for(int i=1; i<currNumRanges; i++){
      curr = sortedInput.ranges[i];
      
      // printf("%d, %d:  %d\n", prev.lower, curr.lower, range_distance(prev, curr));
      if(distance < 0 || abs(range_distance(prev, curr)) < distance){
        distance = abs(range_distance(prev, curr));
        prevIndex = i;
        currIndex = i+1;
      }
      prev = curr;
    }

    Int4Range toInsert = {
      sortedInput.ranges[prevIndex].lower < sortedInput.ranges[currIndex].lower ? sortedInput.ranges[prevIndex].lower : sortedInput.ranges[currIndex].lower,
      sortedInput.ranges[prevIndex].upper > sortedInput.ranges[currIndex].upper ? sortedInput.ranges[prevIndex].upper : sortedInput.ranges[currIndex].upper
    };

    sortedInput.ranges[currIndex] = toInsert;
    // printf("%d, %d", sortedInput.ranges[currIndex].lower, sortedInput.ranges[currIndex].upper);
    
    for(int i=currIndex; i<currNumRanges; i++){
      sortedInput.ranges[i] = sortedInput.ranges[i+1];
    }

    currNumRanges -=1;
  }

  return sortedInput;
}

int main(){

  Int4Range sorted_ranges[] = {{1,10}, {12,13}, {100,300}, {505,700}, {5,10}};
  Int4Range unsort_ranges[] = {{12,13}, {100,300}, {1,10}, {505,700}, {5,10}};
  Int4Range minisorted_ranges[] = {{1,10}, {12,13}, {100,300}};
  
  // Int4RangeSet temp = {minisorted_ranges, 4};
  Int4RangeSet temp = {sorted_ranges, 4};

  Int4RangeSet res = reduceSize(temp, 3);

  printf("output count: %d\n", res.count);
  for(int i=0; i<res.count; i++){
    printf("%d, %d \n", res.ranges[i].lower, res.ranges[i].upper);
  }

  // Int4Range r1 = {0,3};
  // Int4Range r2 = {4,6};
  // Int4Range r3 = {10,1000};
  // Int4Range r4 = {-100,-10};

  // printf("%d\n", range_distance(r1, r2));
  // printf("%d\n", range_distance2(r2, r1));
  // printf("%d\n", range_distance(r1, r3));
  // printf("%d\n", range_distance(r1, r4));

  return 0;
}