#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "helperFunctions.h" 

// // [Inclusive LB, Exclusive UB)
// typedef struct{ 
//     int lower; // inclusive
//     int upper; // exclusive
// } Int4Range;

// // array of Int4Range's and the tot count of array
// typedef struct{
//     Int4Range* ranges;
//     size_t count;
// } Int4RangeSet;

// // [Inlcusive LB, Inclusive UB]
// typedef struct{
//     int lower;  //inclusive
//     int upper;  //inclusive
// } Multiplicity;

int MIN(int My_array[], int len) {
  int num = My_array[0];
  for (int i = 1; i < len; i++) {
    if (My_array[i] < num) {
      num = My_array[i];
    }
  }
  return num;
}
int MAX(int My_array[], int len) {
  int num = My_array[0];
  for (int i = 1; i < len; i++) {
    if (My_array[i] > num) {
      num = My_array[i];
    }
  }
  return num;
}

void printRange(Int4Range a){
  if (a.isNull){
    printf("NULL\n");
  }
  else{
    printf("[");
    printf("%d, %d", a.lower, a.upper);
    printf(")\n");
  }
}


void printRangeSet(Int4RangeSet a){
  printf("{");
  
  for (size_t i=0; i<a.count; i++){
    if (a.ranges[i].isNull) {
      printf(" NULL ");
    }
    else {
      printf(" [%d, %d)", a.ranges[i].lower, a.ranges[i].upper);
    }
  }
  printf("}\n");
}




Int4RangeSet range_set_multiply(Int4RangeSet a, Int4RangeSet b){
    Int4RangeSet rv = {NULL, 0};
    
    if (a.count == 0 || b.count == 0) return rv;
    
    rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    // might need to fix checks
    if (!rv.ranges){
        rv.ranges = NULL;
        rv.count = 0;
        return rv;
    }

    rv.count = (a.count*b.count);
    size_t idx = 0;
    for (size_t i=0; i<a.count; i++){
        for (size_t j=0; j<b.count; j++){
            int arr[] = {
                a.ranges[i].lower * b.ranges[j].lower, 
                a.ranges[i].lower * (b.ranges[j].upper-1), 
                (a.ranges[i].upper-1) * b.ranges[j].lower, 
                (a.ranges[i].upper-1) * (b.ranges[j].upper-1)
            };
            rv.ranges[idx].lower = MIN(arr, 4);
            rv.ranges[idx].upper = MAX(arr, 4)+1;
            idx++;
        }
    }
    return rv;
}

/*
  Helper function to multiply float interval set * float interval set. 
  Does not handle edge cases like mult = (0,n), or a is empty. This is handled caller function
*/ 
Int4Range floatIntervalSetMult(Int4RangeSet a, Multiplicity mult) {
    // convert mult to I4R... make exlusive upper
    Int4RangeSet convertedMult = {NULL, 1};
    convertedMult.ranges = malloc(sizeof(Int4RangeSet));
    Int4Range convertedMultRange = {mult.lower, mult.upper+1};
    convertedMult.ranges[0] = convertedMultRange;

    // create return value and update inplace
    Int4Range expanded;

    int minLB = mult.lower;
    int maxUB = mult.upper+1;
    for (size_t j=0; j < a.count; j++){
        int r1 = convertedMultRange.lower * a.ranges[j].lower;
        int r2 = convertedMultRange.lower * (a.ranges[j].upper-1); 
        int r3 = (convertedMultRange.upper-1) * a.ranges[j].lower;
        int r4 = (convertedMultRange.upper-1) * (a.ranges[j].upper-1);
        int arr[] = {r1, r2, r3, r4, minLB, maxUB};
        
        minLB = MIN(arr, 6);
        maxUB = MAX(arr, 6);
    }
    
    Int4Range rv = {minLB, maxUB+1};
    return rv;
}


/*
  Helper function to multiply float interval set * float interval set. 
  Does not handle edge cases like mult = (0,n), or a is empty. This is handled caller function
*/ 
Int4Range floatIntervalSetMult2(Int4RangeSet a, Multiplicity mult) {
    Int4RangeSet convertedMult = {NULL, 1};
    convertedMult.ranges = malloc(sizeof(Int4RangeSet));
    Int4Range convertedMultRange = {mult.lower, mult.upper+1};
    convertedMult.ranges[0] = convertedMultRange;

    Int4RangeSet expanded = range_set_multiply(a, convertedMult);  

    int minLB = expanded.ranges[0].lower;
    int maxUB = expanded.ranges[0].upper;
    for (size_t i=1; i<expanded.count; i++) {
        if (minLB > expanded.ranges[i].lower) {
            minLB = expanded.ranges[i].lower;
        }
        else if (maxUB < expanded.ranges[i].upper) {
            maxUB = expanded.ranges[i].upper;
        }
    }
    Int4Range rv = {minLB, maxUB};
    return rv;
}


// Int4RangeSet testNormalize(Int4RangeSet set1) {
//     // printf("%d\n\n", set1.count);
//     // printRangeSet(set1);
  
//     Int4RangeSet rv;
//     rv.ranges = malloc(sizeof(Int4Range) * set1.count);

//     int currIdx = 0;
//     for (int i=0; i<set1.count; i++) {
//       // not empty then copy
//       if (set1.ranges[i].lower != 0 || set1.ranges[i].upper != 0) {
//         // printRange(set1.ranges[i]);
//         rv.ranges[currIdx++] = set1.ranges[i];

//       } 
//     }
//     rv.count = currIdx;

//     // Int4Range *temp = realloc(rv.ranges, sizeof(Int4Range) * rv.count);
//     if (rv.count == 0) {
//       free(rv.ranges);
//       rv.ranges = NULL;
//     }
//     else {
//       Int4Range *temp =
//       realloc(rv.ranges, sizeof(Int4Range) * rv.count);
      
//       if (temp != NULL) rv.ranges = temp;
//     }
    
//     // printf("%d\n\n", rv.count);
//     // printRangeSet(rv);


//     // now normalize range
//     Int4RangeSet norm_rv = normalize(rv);
    
//     printRangeSet(norm_rv);

//     return rv;
// } 


// Int4Range range_add(Int4Range a, Int4Range b){
//     // check that either operand is NULL
//     if (a.isNull) return b;
//     else if (b.isNull) return a;
    
//     Int4Range rv;
    
//     rv.lower = 0;
//     rv.upper = 0;

//     // postgres raises error on invalid range input. Not sure if this check is useful
//     // if (!validRange(a) || !validRange(b)){
//     //     return rv;
//     // }

//     rv.lower = a.lower+b.lower;
//     rv.upper = a.upper+b.upper-1; // exclusive upper
//     return rv;
// }

// Int4RangeSet range_set_add(Int4RangeSet a, Int4RangeSet b){
//     Int4RangeSet rv;
//     size_t idx;
//     size_t i;
//     size_t j;

//     rv.ranges = NULL;
//     rv.count = 0;
//     rv.containsNull = a.containsNull && b.containsNull ? true : false;  // NULL {+,-,/,*} NULL == NULL

//     // check for 2 sets with only NULL in them
//     if (a.count == 1 && b.count == 1 && rv.containsNull) return rv;
    
//     rv.ranges = malloc(sizeof(Int4Range) * (a.count * b.count));
    
//     // // might need to fix checks
//     // if (!rv.ranges){
//     //     rv.ranges = NULL;
//     //     rv.count = 0;
//     //     rv.containsNull = false
//     //     return rv;
//     // }

//     rv.count = (a.count*b.count);
//     idx = 0;
//     for (i=0; i<a.count; i++){
//         for (j=0; j<b.count; j++){
//             int currLow;
//             int currHigh;

//             // if (a.ranges[i].isNull && b.ranges[j].isNull) {

//             // }
//             // ignore first range if null
//             if (a.ranges[i].isNull) {
//                 currLow = b.ranges[j].lower;
//                 currHigh = (b.ranges[j].upper); //exclusive upper    
//             }
//             // ignore second range if null
//             else if (b.ranges[j].isNull) {
//                 currLow = a.ranges[i].lower;
//                 currHigh = (a.ranges[i].upper); //exclusive upper    
//             }
//             // neither ranges are null, normal calculation
//             else {
//                 currLow = a.ranges[i].lower + b.ranges[j].lower;
//                 currHigh = (a.ranges[i].upper + b.ranges[j].upper)-1; //exclusive upper
//             }

//             // assign proper result values, and null flag
//             rv.ranges[idx].lower = currLow;
//             rv.ranges[idx].upper = currHigh;
//             rv.ranges[idx].isNull = a.ranges[i].isNull && b.ranges[j].isNull ? true : false;
//             idx++;
//         }
//     }
//     return rv;
// }


int main(){
  Int4Range a = {1,3};
  Int4Range b = {10,21};
  Int4Range c = {5,8};
  Int4Range d = {7,12};
  Int4Range e = {0,0};
  Int4Range f = {6,11};

  Int4Range n;
  n.isNull = true;


  Multiplicity mult1 = {0,1};
  Multiplicity mult2 = {1,2};
  Multiplicity mult3 = {0,2};

  Int4Range a_ranges[] = {a, n, f};
  Int4Range b_ranges[] = {b, n};
  Int4RangeSet s1 = {a_ranges, 3, true};
  Int4RangeSet s2 = {b_ranges, 2, true};
  
  Int4RangeSet rv1 = range_set_add(s1, s2); 
  printRangeSet(rv1);

  // Int4Range rv1 = floatIntervalSetMult(s1, mult1);  
  // printRange(rv1);
  // Int4Range rv2 = floatIntervalSetMult(s1, mult2);  
  // printRange(rv2);
  // Int4Range rv3 = floatIntervalSetMult(s1, mult3);  
  // printRange(rv3);


  // Int4Range c_ranges[] = {f, a, e, c};
  // Int4RangeSet s3 = {c_ranges, 4};
  // Int4RangeSet tn_rv = testNormalize(s3);
  // printRangeSet(tn_rv);


  return 0;
}

