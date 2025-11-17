#ifndef HELPER_FUNCTION_H
#define HELPER_FUNCTION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// #include "postgres.h" /////

// maybe split into own file???
typedef struct{ 
    int lower; // inclusive
    int upper; // exclusive
} Int4Range;

typedef struct{
    Int4Range* ranges;
    size_t count;
} Int4RangeSet;

// add extra utilites for working with defined type
void printRangeSet(Int4RangeSet a);
bool validRange(Int4Range a);

int MIN(int My_array[], int len);
int MAX(int My_array[], int len);

bool overlap(Int4Range a, Int4Range b);
bool contains(Int4Range a, Int4Range b);
int range_distance(Int4Range a, Int4Range b);
Int4Range lift(int x);
Int4RangeSet sort(Int4RangeSet vals);
Int4RangeSet normalize(Int4RangeSet vals);
Int4RangeSet reduceSize(Int4RangeSet vals, int numRangesKeep);


#endif