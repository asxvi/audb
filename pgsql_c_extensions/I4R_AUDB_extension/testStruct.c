#include <stdio.h>
#include <stdbool.h>

typedef struct Int4Range{ 
    int lower; // inclusive
    int upper; // exclusive
} Int4Range;

typedef struct Int4RangeSet{
    Int4Range *ranges;
    size_t count;
}Int4RangeSet;

bool validRange(Int4Range a){
  return a.lower <= a.upper;
}

Int4Range range_add(Int4Range a, Int4Range b){
    Int4Range rv = {0,0};
    if (!validRange(a) || !validRange(b)){
        return rv;
    }

    rv.lower = a.lower+b.lower;
    rv.upper = a.upper+b.upper-1; // exclusive upper
    return rv;
}

int main() {
    Int4Range a = {0,0};
    Int4Range b = {1,0};
    Int4Range c = {2,2};
    Int4Range d;

    Int4Range sol1 = range_add(a, b);
    Int4Range sol2 = range_add(b, b);
    Int4Range sol3 = range_add(a, d);

    printf("%d, %d\n", sol1.lower, sol1.upper);
    printf("%d, %d\n", sol2.lower, sol2.upper);
    printf("%d, %d\n", sol3.lower, sol3.upper);

    
    Int4RangeSet rv;
    rv.count = 3;
    Int4Range ranges[] = {a, b, c};
    return 0;
}