#ifndef HELPER_FUNCTION_H
#define HELPER_FUNCTION_H

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

// add extra utilites for working with defined type
void printRangeSet(Int4RangeSet a);
bool validRange(Int4Range a);

int MIN(int My_array[], int len);
int MAX(int My_array[], int len);

Int4Range lift(int x);
Int4RangeSet sort(Int4RangeSet vals);
Int4RangeSet normalize(Int4RangeSet vals);


#endif