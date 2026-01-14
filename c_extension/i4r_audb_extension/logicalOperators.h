#ifndef LOGICALOPERATORS_H
#define LOGICALOPERATORS_H

#include "helperFunctions.h"

// 3VL 1 true, -1 null, 0 false 
int range_less_than(Int4RangeSet a, Int4RangeSet b);
int range_less_than_equal(Int4RangeSet a, Int4RangeSet b);
int range_greater_than(Int4RangeSet a, Int4RangeSet b);
int range_greater_than_equal(Int4RangeSet a, Int4RangeSet b);

#endif