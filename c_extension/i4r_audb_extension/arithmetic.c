#include "postgres.h"
#include <stdlib.h>
#include <stdio.h>
#include "arithmetic.h"

#define malloc palloc       // must include postgres for palloc
#define free pfree

Int4Range range_add(Int4Range a, Int4Range b){
    Int4Range       rv;
    
    rv.lower = 0;
    rv.upper = 0;

    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    rv.lower = a.lower+b.lower;
    rv.upper = a.upper+b.upper-1; // exclusive upper
    return rv;
}

Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet    rv;
    size_t          idx;
    size_t          i;
    size_t          j;

    rv.ranges = NULL;
    rv.count = 0;

    if (a.count == 0 || b.count == 0) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    // might need to fix checks
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }

    rv.count = (a.count*b.count);
    idx = 0;
    for (i=0; i<a.count; i++){
        for (j=0; j<b.count; j++){
            rv.ranges[idx].lower = a.ranges[i].lower + b.ranges[j].lower;
            rv.ranges[idx].upper = (a.ranges[i].upper + b.ranges[j].upper)-1; //exclusive upper
            idx++;
        }
    }
    return rv;
}

Int4Range range_subtract(Int4Range a, Int4Range b){
    Int4Range       rv;
    
    rv.lower = 0;
    rv.upper = 0;

    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    // account for exclusive upper with -1
    rv.lower = a.lower - (b.upper-1);
    rv.upper = ((a.upper-1) - b.lower) + 1; 
    return rv;
}

Int4RangeSet range_set_subtract(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet    rv;
    size_t          idx;
    size_t          i;
    size_t          j;

    rv.ranges = NULL;
    rv.count = 0;
    
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
    Int4Range       rv;
    rv.lower = 0;
    rv.upper = 0;
    
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

Int4Range floatIntervalSetMult(Int4RangeSet a, Multiplicity mult) {
    // convert mult to I4R... make exlusive upper
    Int4RangeSet convertedMult = {NULL, 1};
    convertedMult.ranges = malloc(sizeof(Int4RangeSet));
    Int4Range convertedMultRange = {mult.lower, mult.upper+1};
    convertedMult.ranges[0] = convertedMultRange;

    // create return value and update inplace
    Int4Range expanded;

    int minLB = mult.lower;
    int maxUB = mult.upper+1;
    for (size_t j=0; j < a.count; j++){
        int r1 = convertedMultRange.lower * a.ranges[j].lower;
        int r2 = convertedMultRange.lower * (a.ranges[j].upper-1); 
        int r3 = (convertedMultRange.upper-1) * a.ranges[j].lower;
        int r4 = (convertedMultRange.upper-1) * (a.ranges[j].upper-1);
        int arr[] = {r1, r2, r3, r4, minLB, maxUB};
        
        minLB = MIN(arr, 6);
        maxUB = MAX(arr, 6);
    }
    
    Int4Range rv = {minLB, maxUB+1};
    return rv;
}

// divison with a bound crossing 0 should be 0 or ???
Int4Range range_divide(Int4Range a, Int4Range b){
    Int4Range       rv;
    int arr[4];
    
    rv.lower = 0;
    rv.upper = 0;

    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    arr[0] = a.lower / b.lower;
    arr[1] = a.lower / (b.upper-1);
    arr[2] = (a.upper-1) / b.lower;
    arr[3] = (a.upper-1) / (b.upper-1);

    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4)+1;
    return rv;
}

Int4RangeSet range_set_divide(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet    rv;
    size_t          idx;
    size_t          i;
    size_t          j;
    
    rv.ranges = NULL;
    rv.count = 0;

    if (a.count == 0 || b.count == 0) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    // might need to fix checks
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }

    rv.count = (a.count*b.count);
    idx = 0;
    
    for (i=0; i<a.count; i++){
        for (j=0; j<b.count; j++){
        int         arr[4];

            arr[0] = (a.ranges[i].lower) / (b.ranges[j].lower);
            arr[1] = (a.ranges[i].lower) / (b.ranges[j].upper-1);
            arr[2] = (a.ranges[i].upper-1) / (b.ranges[j].lower);
            arr[3] = (a.ranges[i].upper-1) / (b.ranges[j].upper-1);
            
            rv.ranges[idx].lower = MIN(arr, 4);
            rv.ranges[idx].upper = MAX(arr, 4)+1;
            idx++;
        }
    }
    return rv;
}
