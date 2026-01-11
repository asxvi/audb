#include "postgres.h"
#include <stdlib.h>
#include <stdio.h>
#include "arithmetic.h"

#define malloc palloc       // must include postgres for palloc
#define free pfree

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

Int4RangeSet range_set_multiply(Int4RangeSet a, Int4RangeSet b){
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

// divison with a bound crossing 0 should be 0 or ???
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
