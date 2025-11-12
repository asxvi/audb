#ifndef ARITHMETIC_H
#define ARITHMETIC_H

// postgres extension headers
# include "postgres.h"
#include "fmgr.h"
#include "varatt.h"
#include "helperFunctions.h"

// typedef struct{ 
//     int lower; // inclusive
//     int upper; // exclusive
// } Int4Range;
// typedef struct{
//     Int4Range* ranges;
//     size_t count;
// } Int4RangeSet;

// add extra utilites for working with defined type
void printRangeSet(Int4RangeSet a);
bool validRange(Int4Range a);
int MIN(int My_array[], int len);
int MAX(int My_array[], int len);

// Range functions
Int4Range range_add(Int4Range a, Int4Range b);
Int4Range range_subtract(Int4Range a, Int4Range b);
Int4Range range_multiply(Int4Range a, Int4Range b);
Int4Range range_divide(Int4Range a, Int4Range b);

// Set functions
Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_subtract(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_multiply(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_divide(Int4RangeSet a, Int4RangeSet b);
#endif
