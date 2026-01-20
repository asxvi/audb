#include "postgres.h"
#include "helperFunctions.h"
#include "arithmetic.h" //
#include <string.h>

#define malloc palloc
#define free pfree

// #define min2(a, b) (((a) < (b)) ? (a) : (b))
// #define max2(a, b) (((a) > (b)) ? (a) : (b))


// https://www.delftstack.com/howto/c/c-max-and-min-function/
// linearly scan to find the MIN value in array
int MIN(int My_array[], int len) {
  int num;
  int i;
  
  num = My_array[0];
  for (i = 1; i < len; i++) {
    if (My_array[i] < num) {
      num = My_array[i];
    }
  }
  return num;
}

// linearly scan to find the MAX value in array
int MAX(int My_array[], int len) {
  int num;
  int i;

  num = My_array[0];
  for (i = 1; i < len; i++) {
    if (My_array[i] > num) {
      num = My_array[i];
    }
  }
  return num;
}

// Print I4R type
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

// Print I4RSet type
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

// Checks if a is increasing: a.lower <= a.upper
bool validRange(Int4Range a){
  return a.lower <= a.upper;
}

// Checks if a is stricly increasing: a.lower < a.upper
bool validRangeStrict(Int4Range a){
  return a.lower < a.upper;
}

// lift a scalar int into a Int4Range
Int4Range lift_scalar(int x){
  Int4Range rv;
  rv.lower = x;
  rv.upper = x+1;
  
  return rv;
}

// lift a single Int4Range into a Int4RangeSet
Int4RangeSet lift_range(Int4Range a){
  Int4RangeSet rv;
  rv.count = 1;
  rv.ranges = malloc(sizeof(Int4Range));
  rv.ranges[0] = a;

  return rv;
}

/*Return Int4RangeSet of the non reduced result of taking every range in a and b
  and finding the min result range:= {min(aL, bL), min(aU, bU) a x b}
  
  Room for optimization avoiding non overlapping comparisons
*/
Int4RangeSet min_rangeSet(Int4RangeSet a, Int4RangeSet b){
  Int4RangeSet rv;
  Int4Range newRange;
  int i;
  int j;

  rv.ranges = malloc(sizeof(Int4Range)*(a.count*b.count));   // worst case scenario, no overlap
  rv.count = 0;
  
  for(i=0; i<a.count; i++){
    for(j=0; j<b.count; j++){
      newRange.lower = min2(a.ranges[i].lower, b.ranges[j].lower);
      newRange.upper = min2(a.ranges[i].upper, b.ranges[j].upper);
      rv.ranges[rv.count++] = newRange;
    } 
  }
  return rv;
}

/*Return Int4RangeSet of the non reduced result of taking every range in a and b
  and finding the max result range:= {max(aL, bL), max(aU, bU) a x b}

  Room for optimization avoiding non overlapping comparisons
*/
Int4RangeSet max_rangeSet(Int4RangeSet a, Int4RangeSet b){
  Int4RangeSet rv;
  Int4Range newRange;
  int i;
  int j;

  rv.ranges = malloc(sizeof(Int4Range)*(a.count*b.count));   // worst case scenario, no overlap
  rv.count = 0;
  
  for(i=0; i<a.count; i++){
    for(j=0; j<b.count; j++){
      newRange.lower = max2(a.ranges[i].lower, b.ranges[j].lower);
      newRange.upper = max2(a.ranges[i].upper, b.ranges[j].upper);
      rv.ranges[rv.count++] = newRange;
    } 
  }
  return rv;
}

// https://www.geeksforgeeks.org/c/comparator-function-of-qsort-in-c/#
static int q_sort_compare_ranges(const void* range1, const void* range2){
  Int4Range r1;
  Int4Range r2;
  r1 = *(Int4Range*)range1;
  r2 = *(Int4Range*)range2;

  if(r1.lower != r2.lower){
    return r1.lower < r2.lower ? -1 : 1;
  }

  return r1.upper < r2.upper ? -1 : 1;
}

// Allocates space for new array that is sorted on 1)lower, 2)upper using quicksort
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
  
  qsort(sorted.ranges, vals.count, sizeof(Int4Range), q_sort_compare_ranges);
  
  return sorted;
}

// Traverses through entire set and looks to merge any possible overlap.
// Allocates space for new array 
Int4RangeSet normalize(Int4RangeSet vals){
  Int4RangeSet normalized;
  Int4RangeSet sorted;
  Int4Range prev;
  size_t i;

  if (vals.count == 0){
    normalized.count = 0;
    normalized.ranges = NULL;
    return normalized;
  }
  
  sorted = sort(vals);
  normalized.count = 0;
  normalized.ranges = malloc(sizeof(Int4Range) * sorted.count);
  
  prev = sorted.ranges[0];

  for(i=1; i<sorted.count; i++){
    Int4Range curr;
    curr = sorted.ranges[i];
    
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
  
  free(sorted.ranges);

  return normalized;
}

// // returns the Int4RangeSet of any intervals not empty 
// Int4RangeSet removeEmpty(Int4RangeSet vals) {
//   Int4RangeSet result = {NULL, 0};

//   if (vals.count == 0) {
//     return result;
//   }

//   size_t nonEmptyCount = 0;
//   for (size_t i = 0; i < vals.count; i++) {
//     if (validRangeStrict(vals.ranges[i])) {
//       nonEmptyCount++;
//     }
//   }

//   if (nonEmptyCount == 0){
//     return result;
//   }

//   // only add in non empty ranges
//   int currIdx = 0;
//   result.ranges = malloc(sizeof(Int4Range) * nonEmptyCount);
//   for (size_t i = 0; i < vals.count; i++) {
//     if (validRangeStrict(vals.ranges[i])) {
//       result.ranges[currIdx] = vals.ranges[i];
//       currIdx++;
//     }
//   }

//   return result;
// } // free in returning function

// Checks if 2 ranges overlap at all
bool overlap(Int4Range a, Int4Range b){
  return a.lower < b.upper && b.lower < a.upper;
}

// Checks if a fully contains b
bool contains(Int4Range a, Int4Range b){
  return (a.lower <= b.lower && b.lower <= a.upper 
      && a.lower <= b.upper && b.upper <= a.upper);
}

// confusion example: a(1,5) b(2,9)
// Find the distance between 2 ranges
int range_distance(Int4Range a, Int4Range b){
  if ((a.upper-1) <= b.lower){
    return b.lower - (a.upper-1);
  }
  else if ((b.upper-1) <= a.lower){
    return a.lower - (b.upper-1);
  }
  // overlap ranges
  else{
    return 0;
  }
}

// reduce size inplace and return newly allocated RangeSet
Int4RangeSet reduceSize(Int4RangeSet vals, int numRangesKeep){
  Int4RangeSet normalized;
  Int4RangeSet sortedInput;
  int currNumRanges;

  if (vals.count == 0){
    normalized.count = 0;
    normalized.ranges = NULL;
    return normalized;
  }
  else if (vals.count <= numRangesKeep){
    return vals;
  }

  sortedInput = sort(vals);
  currNumRanges = sortedInput.count;

  while(currNumRanges > numRangesKeep){
    int bestDist;
    int bestIndex;
    int currDist;
    int i;
    int j;
    Int4Range a;
    Int4Range b;
    Int4Range toInsert;

    bestDist = -1;
    bestIndex = -1;

    // greedy look for smallest remaining gap
    for(i=1; i<currNumRanges; i++){
      currDist = abs(range_distance(sortedInput.ranges[i], sortedInput.ranges[i-1]));
      
      // compare distances and keep min difference between 2 ranges in entire set
      if(bestDist < 0 || currDist < bestDist){
        bestDist = currDist;
        bestIndex = i-1;
      }
    }
    
    a = sortedInput.ranges[bestIndex];
    b = sortedInput.ranges[bestIndex+1];

    toInsert.lower = (a.lower < b.lower ? a.lower : b.lower);
    toInsert.upper = (a.upper > b.upper ? a.upper : b.upper);

    sortedInput.ranges[bestIndex] = toInsert;

    for (j=bestIndex+1; j<currNumRanges-1; j++){
      sortedInput.ranges[j] = sortedInput.ranges[j+1];
    }

    currNumRanges -= 1;
  }

  sortedInput.count = currNumRanges;

  return sortedInput;
}


// void reallocRangeSet(Int4RangeSet* a){
//   int size;
//   int trueSize 
//   int i;

//   size = a->count;
//   trueSize = 0;
//   for(i=0; i<size; i++){
//     if (a->ranges[i].lower == 0 && a->ranges[i].upper == 0){
//       continue;
//     }
//     trueSize++;
    
//     // https://www.geeksforgeeks.org/c/g-fact-53/
//     // printf("%d, %d", a->ranges[i].lower, a->ranges[i].upper);   // default ig is (0,0)
//   }
// }

// int main(){
//   Int4Range a = {1,2};
//   Int4Range b = {10,11};
//   Int4Range c = {5,6};
//   Int4Range d = {7,12};
  
//   Int4Range a_ranges[] = {a, b};
//   Int4Range b_ranges[] = {c, d};
//   Int4RangeSet s1 = {a_ranges, 2};
//   Int4RangeSet s2 = {b_ranges, 2};

//   Int4RangeSet rv  = max_rangeSet(s1, s2);
//   printRangeSet(rv);

//   rv.count = 50;
//   reallocRangeSet(&rv);

//   return 0;
// }
