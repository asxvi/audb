#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "arithmetic.h"

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
        a.lower * (b.upper-1), 
        (a.upper-1) * b.lower, 
        (a.upper-1) * (b.upper-1)
    };
    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4)+1;
    return rv;
}
Int4RangeSet range_set_mutiply(Int4RangeSet a, Int4RangeSet b){
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
            int arr[] = {
                a.ranges[i].lower * b.ranges[j].lower, 
                a.ranges[i].lower * (b.ranges[j].upper-1), 
                (a.ranges[i].upper-1) * b.ranges[j].lower, 
                (a.ranges[i].upper-1) * (b.ranges[j].upper-1)
            };
            rv.ranges[idx].lower = MIN(arr, 4);
            rv.ranges[idx].upper = MAX(arr, 4)+1;
            idx++;
        }
    }
    return rv;
}

Int4Range range_divide(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    // printf("%d, %d, %d, %d\n", a.lower, a.upper, b.lower, b.upper);
    int arr[] = {
        a.lower / b.lower, 
        a.lower / (b.upper-1), 
        (a.upper-1) / b.lower, 
        (a.upper-1) / (b.upper-1)
    };

    // printf("%d, %d, %d, %d\n", arr[0], arr[1], arr[2], arr[3]);
    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4)+1;
    return rv;
}
Int4RangeSet range_set_divide(Int4RangeSet a, Int4RangeSet b){
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
            int arr[] = {
                a.ranges[i].lower / b.ranges[j].lower, 
                a.ranges[i].lower / (b.ranges[j].upper-1), 
                (a.ranges[i].upper-1) / b.ranges[j].lower, 
                (a.ranges[i].upper-1) / (b.ranges[j].upper-1)
            };
            rv.ranges[idx].lower = MIN(arr, 4);
            rv.ranges[idx].upper = MAX(arr, 4)+1;
            idx++;
        }
    }
    return rv;
}

int main(){
    
    // Int4Range l = lift(6);
    // printf("%d, %d", l.lower, l.upper);
    // Int4Range l_ranges[] = {{9,10}, {12,13}, {1,3}, {5,7}, {5,10}};
    // Int4RangeSet rangeSet = {l_ranges, 5};
    // Int4RangeSet sorted = sort(rangeSet);
    // printf("sorted:  ");
    // printRangeSet(sorted);
    // Int4RangeSet norm = normalize(sorted);
    // printf("normalized:  ");
    // printRangeSet(norm);
    
    // Int4Range rv;
    Int4Range a = {1, 4};
    Int4Range b = {4, 10};
    Int4Range c = {5, 7};
    Int4Range d = {12, 13};
    
    printf("%d", range_distance(a, b));
    printf("%d", range_distance(a, c));
    printf("%d", range_distance(a, d));



    // bool ol = overlap(a, b);
    // printf("%d\n" ,ol);
    
    // Int4Range x = {4, 8};
    // Int4Range y = {2, 3};
    
    // Int4Range a_ranges[] = {{1,3}, {4,5}};
    // Int4Range b_ranges[] = {{6,7}, {8,9}};
    // Int4Range a_ranges[] = {{1,3}, {5,7}};
    // Int4Range b_ranges[] = {{9,10}, {12,13}};
    
    // Int4RangeSet aSet = {a_ranges, 2};
    // Int4RangeSet bSet = {b_ranges, 2}; 
    // Int4RangeSet rvSet;
    // printRangeSet(aSet);
    // printRangeSet(bSet);
    
    // rv = range_add(a, b);
    // printf("add: %d, %d\n", rv.lower, rv.upper);
    // rv = range_subtract(a, b);
    // printf("sub: %d, %d\n", rv.lower, rv.upper);
    // rv = range_multiply(a, b);
    // printf("mult: %d, %d\n", rv.lower, rv.upper);
    // rv = range_divide(a, b);
    // rv = range_divide(x, y);
    // printf("div: %d, %d\n", rv.lower, rv.upper);

    // rvSet = range_set_add(aSet, bSet);
    // printRangeSet(rvSet);
    // printf("%zu\n", rvSet.count);
    // rvSet = range_set_subtract(aSet, bSet);
    // printRangeSet(rvSet);
    // printf("%zu\n", rvSet.count);
    // rvSet = range_set_mutiply(aSet, bSet);
    // printRangeSet(rvSet);
    // printf("%zu\n", rvSet.count);
    // rvSet = range_set_divide(aSet, bSet);
    // printRangeSet(rvSet);
    // printf("%zu\n", rvSet.count);

    return 0;
}

