#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "helperFunctions.h"

// Range functions
Int4Range range_add(Int4Range a, Int4Range b);
Int4Range range_subtract(Int4Range a, Int4Range b);
Int4Range range_multiply(Int4Range a, Int4Range b);
Int4Range range_divide(Int4Range a, Int4Range b);


// Set functions
Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_subtract(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_mutiply(Int4RangeSet a, Int4RangeSet b);
Int4RangeSet range_set_divide(Int4RangeSet a, Int4RangeSet b);

#endif