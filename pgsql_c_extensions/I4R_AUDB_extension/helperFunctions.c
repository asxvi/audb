#include <string.h>
#include "helperFunctions.h"

// #include "postgres.h" /////
// #define malloc palloc
// #define free pfree

#define min2(a, b) (((a) < (b)) ? (a) : (b))
#define max2(a, b) (((a) > (b)) ? (a) : (b))

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

// convenience function
void printRange(Int4Range a){
  printf("[");
  printf("%d, %d", a.lower, a.upper);
  printf(")\n");
}

// convenience function
void printRangeSet(Int4RangeSet a){
  printf("{");
  for (size_t i=0; i<a.count; i++){
    printf("[%d, %d]", a.ranges[i].lower, a.ranges[i].upper);
  }
  printf("}\n");
}

bool validRange(Int4Range a){
  return a.lower <= a.upper;
}

/*lift a scalar int into a Int4Range*/
Int4Range lift_scalar(int x){
  Int4Range rv = {x, x+1};
  return rv;
}

/*lift a single Int4Range into a Int4RangeSet*/
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
  rv.ranges = malloc(sizeof(Int4Range)*(a.count*b.count));   // worst case scenario, no overlap
  rv.count = 0;
  
  Int4Range newRange;
  for(int i=0; i<a.count; i++){
    for(int j=0; j<b.count; j++){
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
  rv.ranges = malloc(sizeof(Int4Range)*(a.count*b.count));   // worst case scenario, no overlap
  rv.count = 0;
  
  Int4Range newRange;
  for(int i=0; i<a.count; i++){
    for(int j=0; j<b.count; j++){
      newRange.lower = max2(a.ranges[i].lower, b.ranges[j].lower);
      newRange.upper = max2(a.ranges[i].upper, b.ranges[j].upper);
      rv.ranges[rv.count++] = newRange;
    } 
  }
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

// using C quicksort to sort on lower, upper allocate a new array
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
  return a.lower < b.upper && b.lower < a.upper;
}

// a contains b
bool contains(Int4Range a, Int4Range b){
  return (a.lower <= b.lower && b.lower <= a.upper 
      && a.lower <= b.upper && b.upper <= a.upper);
}

// // confusion example: a(1,5) b(2,9)
// int range_distance2(Int4Range a, Int4Range b){
//   if(contains(a, b) || contains(b, a)){
//     return 0;
//   }
//   else if((a.upper-1) < b.lower){
//     return (b.lower - (a.upper-1));
//   }
//   else {
//     return (a.lower - (b.upper-1));
//   }
// }

// confusion example: a(1,5) b(2,9)
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
  if (vals.count == 0){
    normalized.count = 0;
    normalized.ranges = NULL;
    return normalized;
  }
  else if (vals.count <= numRangesKeep){
    return vals;
  }

  Int4RangeSet sortedInput = sort(vals);
  int currNumRanges = sortedInput.count;

  while(currNumRanges > numRangesKeep){
    int bestDist = -1;
    int bestIndex = -1;

    // greedy look for smallest remaining gap
    for(int i=1; i<currNumRanges; i++){
      int currDist = abs(range_distance(sortedInput.ranges[i], sortedInput.ranges[i-1]));
      
      // compare distances and keep min difference between 2 ranges in entire set
      if(bestDist < 0 || currDist < bestDist){
        bestDist = currDist;
        bestIndex = i-1;
      }
    }
    
    Int4Range a = sortedInput.ranges[bestIndex];
    Int4Range b = sortedInput.ranges[bestIndex+1];

    Int4Range toInsert = {
      .lower = (a.lower < b.lower ? a.lower : b.lower),
      .upper = (a.upper > b.upper ? a.upper : b.upper)
    };

    sortedInput.ranges[bestIndex] = toInsert;

    for (int i=bestIndex+1; i<currNumRanges-1; i++){
      sortedInput.ranges[i] = sortedInput.ranges[i+1];
    }

    currNumRanges -= 1;
  }

  sortedInput.count = currNumRanges;

  return sortedInput;
}


void reallocRangeSet(Int4RangeSet* a){
  int size = a->count;
  int trueSize = 0;
  for(int i=0; i<size; i++){
    if (a->ranges[i].lower == 0 && a->ranges[i].upper == 0){
      continue;
    }
    trueSize++;
    
    // https://www.geeksforgeeks.org/c/g-fact-53/
    // printf("%d, %d", a->ranges[i].lower, a->ranges[i].upper);   // default ig is (0,0)
  }
}

int main(){
  Int4Range a = {1,2};
  Int4Range b = {10,11};
  Int4Range c = {5,6};
  Int4Range d = {7,12};
  
  Int4Range a_ranges[] = {a, b};
  Int4Range b_ranges[] = {c, d};
  Int4RangeSet s1 = {a_ranges, 2};
  Int4RangeSet s2 = {b_ranges, 2};

  Int4RangeSet rv  = max_rangeSet(s1, s2);
  printRangeSet(rv);

  rv.count = 50;
  reallocRangeSet(&rv);

  return 0;
}
