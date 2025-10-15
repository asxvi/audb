#include <arithmetic.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{ 
    int lower; // inclusive
    int upper; // exclusive
} Int4Range;

typedef struct{
    Int4Range* ranges;
    size_t count;
} Int4RangeSet;

bool validRange(Int4Range a){
    return a.lower < a.upper;
}

// can also implement macro
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



Int4Range range_add(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    rv.lower = a.lower+b.lower;
    rv.upper = a.upper+b.upper-1; // exclusive upper
    return rv;
}
Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet rv = {NULL, 0};
    
    if (a.count == 0 || b.count == 0) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    // might need to fix checks
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }

    rv.count = (a.count*b.count);
    size_t idx = 0;
    for (size_t i=0; i<a.count; i++){
        for (size_t j=0; j<b.count; j++){
            rv.ranges[idx].lower = a.ranges[i].lower + b.ranges[j].lower;
            rv.ranges[idx].upper = (a.ranges[i].upper + b.ranges[j].upper)-1; //exclusive upper
            idx++;
        }
    }
    return rv;
}

Int4Range range_subtract(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    // account for exclusive upper with -1
    rv.lower = a.lower - (b.upper-1);
    rv.upper = ((a.upper-1) - b.lower) + 1; 
    return rv;
}
Int4RangeSet range_set_subtract(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet rv = {NULL, 0};
    
    if (a.count == 0 || b.count == 0) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    // might need to fix checks
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }

    rv.count = (a.count*b.count);
    size_t idx = 0;
    for (size_t i=0; i<a.count; i++){
        for (size_t j=0; j<b.count; j++){
            rv.ranges[idx].lower = a.ranges[i].lower - (b.ranges[j].upper-1);
            rv.ranges[idx].upper = ((a.ranges[i].upper-1) - b.ranges[j].lower) + 1;
            idx++;
        }
    }
    return rv;
}




Int4Range range_multiply(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    int arr[] = {
        a.lower * b.lower, 
        a.lower * b.upper, 
        a.upper * b.lower, 
        a.upper * b.upper
    };
    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4);
    return rv;
}

Int4Range range_divide(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    int arr[] = {
        a.lower / b.lower, 
        a.lower / b.upper, 
        a.upper / b.lower, 
        a.upper / b.upper
    };
    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4);
    return rv;
}

int main(){
    
    Int4Range rv;
    Int4Range a = {1, 3};
    Int4Range b = {9, 10};
    Int4Range c = {5, 7};
    Int4Range d = {12, 13};
    
    // Int4Range a_ranges[] = {{1,3}, {4,5}};
    // Int4Range b_ranges[] = {{6,7}, {8,9}};
    Int4Range a_ranges[] = {{1,3}, {5,7}};
    Int4Range b_ranges[] = {{9,10}, {12,13}};
    
    Int4RangeSet aSet = {a_ranges, 2};
    Int4RangeSet bSet = {b_ranges, 2}; 
    Int4RangeSet rvSet;
    // printRangeSet(aSet);
    // printRangeSet(bSet);
    
    rv = range_add(a, b);
    // printf("add: %d, %d\n", rv.lower, rv.upper);
    // rv = range_subtract(a, b);
    // printf("sub: %d, %d\n", rv.lower, rv.upper);
    // rv = range_multiply(a, b);
    // printf("mult: %d, %d\n", rv.lower, rv.upper);
    // rv = range_divide(a, b);
    // printf("div: %d, %d\n", rv.lower, rv.upper);

    rvSet = range_set_add(aSet, bSet);
    printRangeSet(rvSet);
    printf("%zu\n", rvSet.count);

    rvSet = range_set_subtract(aSet, bSet);
    printRangeSet(rvSet);
    printf("%zu\n", rvSet.count);

    return 0;
}

