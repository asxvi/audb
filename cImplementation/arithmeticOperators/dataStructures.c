#include <dataStructures.h>
#include <stdio.h>
#include <stdlib.h>

void printRangeSet(Int4RangeSet a){
    printf("{");
    for (size_t i=0; i<a.count; i++){
        printf("[%d, %d]", a.ranges[i].lower, a.ranges[i].upper);
    }
    printf("}\n");
}

