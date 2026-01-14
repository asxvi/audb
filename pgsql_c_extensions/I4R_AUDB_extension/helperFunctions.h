#ifndef HELPER_FUNCTION_H
#define HELPER_FUNCTION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define min2(a, b) (((a) < (b)) ? (a) : (b))
#define max2(a, b) (((a) > (b)) ? (a) : (b))

// [Inclusive LB, Exclusive UB)
typedef struct{ 
    int lower; // inclusive
    int upper; // exclusive
} Int4Range;

// array of Int4Range's and the tot count of array
typedef struct{
    Int4Range* ranges;
    size_t count;
} Int4RangeSet;

// [Inlcusive LB, Inclusive UB]
typedef struct{
    int lower;  //inclusive
    int upper;  //inclusive
} Multiplicity;

// add extra utilites for working with defined type
void printRange(Int4Range a);
void printRangeSet(Int4RangeSet a);
bool validRange(Int4Range a);
bool validRangeStrict(Int4Range a);

// for finding the min int in array. probably not optimal method
int MIN(int My_array[], int len);
int MAX(int My_array[], int len);

// for aggregate functions
Int4Range min_range(Int4Range a, Int4Range b);
Int4Range max_range(Int4Range a, Int4Range b);
Int4RangeSet min_rangeSet(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet max_rangeSet(Int4RangeSet a, Int4RangeSet b);

Int4RangeSet floatIntervalSetMult(Int4RangeSet a, Multiplicity mult);
bool overlap(Int4Range a, Int4Range b);
bool contains(Int4Range a, Int4Range b);
int range_distance(Int4Range a, Int4Range b);
Int4Range lift_scalar(int x);
Int4RangeSet lift_range(Int4Range a);
Int4RangeSet sort(Int4RangeSet vals);
Int4RangeSet normalize(Int4RangeSet vals);
Int4RangeSet reduceSize(Int4RangeSet vals, int numRangesKeep);

void reallocRangeSet(Int4RangeSet* a);

#endif