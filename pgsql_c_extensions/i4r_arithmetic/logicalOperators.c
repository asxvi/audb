// #include "logicalOperators.h"

// // 3VL 1 true, 0 null, -1 false 

// int range_greater_than(Int4RangeSet a, Int4RangeSet b){
//     if (a.count == 0 || b.count == 0){
//         return 0;
//     }
    
//     Int4RangeSet n1 = normalize(a);
//     Int4RangeSet n2 = normalize(b);
//     Int4Range f1 = n1.ranges[0];
//     Int4Range f2 = n2.ranges[0];
//     Int4Range l1 = n1.ranges[n1.count-1];
//     Int4Range l2 = n2.ranges[n2.count-1];

//     if (f1.lower > (l2.upper-1)){
//         return 1;
//     }
//     else if ((l1.upper-1) < f2.lower){
//         return -1;
//     }
//     free(n1.ranges);
//     free(n2.ranges);

//     return 0;
// }

// int range_greater_than_equal(Int4RangeSet a, Int4RangeSet b){
//     if (a.count == 0 || b.count == 0){
//         return 0;
//     }
    
//     Int4RangeSet n1 = normalize(a);
//     Int4RangeSet n2 = normalize(b);
//     Int4Range f1 = n1.ranges[0];
//     Int4Range f2 = n2.ranges[0];
//     Int4Range l1 = n1.ranges[n1.count-1];
//     Int4Range l2 = n2.ranges[n2.count-1];

//     if (f1.lower >= (l2.upper-1)){
//         return 1;
//     }
//     else if ((l1.upper-1) < f2.lower){
//         return -1;
//     }
//     return 0;
// }

// int range_less_than(Int4RangeSet a, Int4RangeSet b){
//     if (a.count == 0 || b.count == 0){
//         return 0;
//     }
    
//     Int4RangeSet n1 = normalize(a);
//     Int4RangeSet n2 = normalize(b);
//     Int4Range f1 = n1.ranges[0];
//     Int4Range f2 = n2.ranges[0];
//     Int4Range l1 = n1.ranges[n1.count-1];
//     Int4Range l2 = n2.ranges[n2.count-1];

//     if (f1.lower > (l2.upper-1)){
//         return -1;
//     }
//     else if ((l1.upper-1) < f2.lower){
//         return 1;
//     }
//     return 0;
// }

// int range_less_than_equal(Int4RangeSet a, Int4RangeSet b){
//     if (a.count == 0 || b.count == 0){
//         return 0;
//     }
    
//     Int4RangeSet n1 = normalize(a);
//     Int4RangeSet n2 = normalize(b);
//     Int4Range f1 = n1.ranges[0];
//     Int4Range f2 = n2.ranges[0];
//     Int4Range l1 = n1.ranges[n1.count-1];
//     Int4Range l2 = n2.ranges[n2.count-1];

//     if (f1.lower > (l2.upper-1)){
//         return -1;
//     }
//     else if ((l1.upper-1) <= f2.lower){
//         return 1;
//     }
//     return 0;
// }



#include "logicalOperators.h"

// 3VL 1 true, -1 null, 0 false 

int range_greater_than(Int4RangeSet a, Int4RangeSet b){
    if (a.count == 0 || b.count == 0){
        return -1;
    }
    
    Int4RangeSet n1 = normalize(a);
    Int4RangeSet n2 = normalize(b);
    Int4Range f1 = n1.ranges[0];
    Int4Range f2 = n2.ranges[0];
    Int4Range l1 = n1.ranges[n1.count-1];
    Int4Range l2 = n2.ranges[n2.count-1];

    if (f1.lower > (l2.upper-1)){
        return 1;
    }
    else if ((l1.upper-1) < f2.lower){
        return 0;
    }
    free(n1.ranges);
    free(n2.ranges);

    return -1;
}

int range_greater_than_equal(Int4RangeSet a, Int4RangeSet b){
    if (a.count == 0 || b.count == 0){
        return -1;
    }
    
    Int4RangeSet n1 = normalize(a);
    Int4RangeSet n2 = normalize(b);
    Int4Range f1 = n1.ranges[0];
    Int4Range f2 = n2.ranges[0];
    Int4Range l1 = n1.ranges[n1.count-1];
    Int4Range l2 = n2.ranges[n2.count-1];

    if (f1.lower >= (l2.upper-1)){
        return 1;
    }
    else if ((l1.upper-1) < f2.lower){
        return 0;
    }
    return -1;
}

int range_less_than(Int4RangeSet a, Int4RangeSet b){
    if (a.count == 0 || b.count == 0){
        return -1;
    }
    
    Int4RangeSet n1 = normalize(a);
    Int4RangeSet n2 = normalize(b);
    Int4Range f1 = n1.ranges[0];
    Int4Range f2 = n2.ranges[0];
    Int4Range l1 = n1.ranges[n1.count-1];
    Int4Range l2 = n2.ranges[n2.count-1];

    if (f1.lower > (l2.upper-1)){
        return 0;
    }
    else if ((l1.upper-1) < f2.lower){
        return 1;
    }
    return -1;
}

int range_less_than_equal(Int4RangeSet a, Int4RangeSet b){
    if (a.count == 0 || b.count == 0){
        return -1;
    }
    
    Int4RangeSet n1 = normalize(a);
    Int4RangeSet n2 = normalize(b);
    Int4Range f1 = n1.ranges[0];
    Int4Range f2 = n2.ranges[0];
    Int4Range l1 = n1.ranges[n1.count-1];
    Int4Range l2 = n2.ranges[n2.count-1];

    if (f1.lower > (l2.upper-1)){
        return 0;
    }
    else if ((l1.upper-1) <= f2.lower){
        return 1;
    }
    return -1;
}
