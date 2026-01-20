#include "postgres.h"
#include <stdlib.h>
#include <stdio.h>
#include "arithmetic.h"

#define malloc palloc       // must include postgres for palloc
#define free pfree

Int4Range range_add(Int4Range a, Int4Range b){
    Int4Range rv;

    // check that either operand is NULL
    if (a.isNull) return b;
    else if (b.isNull) return a;    
    
    rv.lower = 0;
    rv.upper = 0;

    // postgres raises error on invalid range input. Not sure if this check is useful
    // if (!validRange(a) || !validRange(b)){
    //     return rv;
    // }

    rv.lower = a.lower+b.lower;
    rv.upper = a.upper+b.upper-1; // exclusive upper
    return rv;
}

Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet rv;
    size_t idx;
    size_t i;
    size_t j;

    rv.ranges = NULL;
    rv.count = 0;
    rv.containsNull = a.containsNull && b.containsNull ? true : false;  // NULL {+,-,/,*} NULL == NULL

    // check for 2 sets with only NULL in them
    if (a.count == 1 && b.count == 1 && rv.containsNull) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    
    // // might need to fix checks
    // if (!rv.ranges){
    //     rv.ranges = NULL;
    //     rv.count = 0;
    //     rv.containsNull = false
    //     return rv;
    // }

    rv.count = (a.count*b.count);
    idx = 0;
    for (i=0; i<a.count; i++){
        for (j=0; j<b.count; j++){
            int currLow;
            int currHigh;

            // ignore first range if null
            if (a.ranges[i].isNull) {
                currLow = b.ranges[j].lower;
                currHigh = (b.ranges[j].upper); //exclusive upper    
            }
            // ignore second range if null
            else if (b.ranges[j].isNull) {
                currLow = a.ranges[i].lower;
                currHigh = (a.ranges[i].upper); //exclusive upper    
            }
            // neither ranges are null, normal calculation
            else {
                currLow = a.ranges[i].lower + b.ranges[j].lower;
                currHigh = (a.ranges[i].upper + b.ranges[j].upper)-1; //exclusive upper
            }

            // assign proper result values, and null flag
            rv.ranges[idx].lower = currLow;
            rv.ranges[idx].upper = currHigh;
            rv.ranges[idx].isNull = a.ranges[i].isNull && b.ranges[j].isNull ? true : false;
            idx++;
        }
    }
    return rv;
}

Int4Range range_subtract(Int4Range a, Int4Range b){
    Int4Range rv;
    
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
    Int4RangeSet rv;
    size_t idx;
    size_t i;
    size_t j;

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
            rv.ranges[idx].lower = a.ranges[i].lower - (b.ranges[j].upper-1);
            rv.ranges[idx].upper = ((a.ranges[i].upper-1) - b.ranges[j].lower) + 1;
            idx++;
        }
    }
    return rv;
}

Int4Range range_multiply(Int4Range a, Int4Range b){
    Int4Range rv;
    int arr[4];
    
    rv.lower = 0;
    rv.upper = 0;
    
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    arr[0] = a.lower * b.lower;
    arr[1] = a.lower * (b.upper-1);
    arr[2] = (a.upper-1) * b.lower;
    arr[3] = (a.upper-1) * (b.upper-1);

    rv.lower = MIN(arr, 4);
    rv.upper = MAX(arr, 4)+1;
    return rv;
}

Int4RangeSet range_set_multiply(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet rv;
    size_t idx;
    size_t i;
    size_t j;
    
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
            int arr[4];

            arr[1] = a.ranges[i].lower * (b.ranges[j].upper-1);
            arr[0] = a.ranges[i].lower * b.ranges[j].lower;
            arr[2] = (a.ranges[i].upper-1) * b.ranges[j].lower;
            arr[3] = (a.ranges[i].upper-1) * (b.ranges[j].upper-1);

            rv.ranges[idx].lower = MIN(arr, 4);
            rv.ranges[idx].upper = MAX(arr, 4)+1;
            idx++;
        }
    }
    return rv;
}

Int4Range floatIntervalSetMult(Int4RangeSet a, Multiplicity mult) {
    Int4RangeSet multSet;
    Int4Range multRange;
    Int4Range rv;
    int minLB;
    int maxUB;
    size_t j;

    // convert mult to I4R... make exlusive upper
    multSet.count = 1;
    multSet.ranges = malloc(sizeof(Int4Range));
    multRange.lower = mult.lower;
    multRange.upper = mult.upper+1;
    multSet.ranges[0] = multRange;

    minLB = mult.lower;
    maxUB = mult.upper+1;
    for (j=0; j < a.count; j++){
        int r1;
        int r2;
        int r3;
        int r4;
        int arr[6];
    
        r1 = multRange.lower * a.ranges[j].lower;
        r2 = multRange.lower * (a.ranges[j].upper-1); 
        r3 = (multRange.upper-1) * a.ranges[j].lower;
        r4 = (multRange.upper-1) * (a.ranges[j].upper-1);
        
        arr[0] = r1;
        arr[1] = r2;
        arr[2] = r3;
        arr[3] = r4;
        arr[4] = minLB;
        arr[5] = maxUB;
        
        minLB = MIN(arr, 6);
        maxUB = MAX(arr, 6);
    }
    
    rv.lower = minLB;
    rv.upper = maxUB + 1;

    free(multSet.ranges); 

    return rv;
}

// divison with a bound crossing 0 should be 0 or ???
Int4Range range_divide(Int4Range a, Int4Range b){
    Int4Range rv;
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
    Int4RangeSet rv;
    size_t idx;
    size_t i;
    size_t j;
    
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
        int arr[4];

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
