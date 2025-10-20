#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

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

#endif