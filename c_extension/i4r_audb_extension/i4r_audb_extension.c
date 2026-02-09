// source files
#include "postgres.h"           // root
#include "fmgr.h"               // "..must be included by all Postgres modules"
#include "utils/rangetypes.h"   // RangeType
#include "utils/array.h"        // ArrayType
#include "utils/typcache.h"     // data type cache
#include "utils/lsyscache.h"    // "Convenience routines for common queries in the system catalog cache."
#include "catalog/pg_type_d.h"  // pg_type oid macros
#include "catalog/namespace.h"  // type helpers

// local code
#include "arithmetic.h"         // logic for arithmetic 
#include "logicalOperators.h"   // logic for logical ops
#include "helperFunctions.h"    // logic for helpers
#include "serialization.h"      // serial and deserial helpers

PG_MODULE_MAGIC;

/*(Arithmetic Functions)*/
PG_FUNCTION_INFO_V1(range_add);
PG_FUNCTION_INFO_V1(range_subtract);
PG_FUNCTION_INFO_V1(range_multiply);
PG_FUNCTION_INFO_V1(range_divide);
PG_FUNCTION_INFO_V1(set_add);
PG_FUNCTION_INFO_V1(set_subtract);
PG_FUNCTION_INFO_V1(set_multiply);
PG_FUNCTION_INFO_V1(set_divide);

/*(Logical Operator Functions)*/
PG_FUNCTION_INFO_V1(range_lt);
PG_FUNCTION_INFO_V1(range_lte);
PG_FUNCTION_INFO_V1(range_gt);
PG_FUNCTION_INFO_V1(range_gte);
PG_FUNCTION_INFO_V1(range_eq);
PG_FUNCTION_INFO_V1(set_lt);
PG_FUNCTION_INFO_V1(set_lte);
PG_FUNCTION_INFO_V1(set_gt);
PG_FUNCTION_INFO_V1(set_gte);
PG_FUNCTION_INFO_V1(set_eq);

/*(Helper Functions)*/
PG_FUNCTION_INFO_V1(lift_scalar);
PG_FUNCTION_INFO_V1(set_sort);
PG_FUNCTION_INFO_V1(set_normalize);
PG_FUNCTION_INFO_V1(set_reduce_size);

/*(Aggregate Functions)*/
//sum
PG_FUNCTION_INFO_V1(combine_range_mult_sum);
PG_FUNCTION_INFO_V1(agg_sum_transfunc);
PG_FUNCTION_INFO_V1(combine_set_mult_sum);
PG_FUNCTION_INFO_V1(agg_sum_set_transfunc);
PG_FUNCTION_INFO_V1(agg_sum_set_finalfunc);

// min/max
PG_FUNCTION_INFO_V1(combine_range_mult_min);
PG_FUNCTION_INFO_V1(combine_range_mult_max);
PG_FUNCTION_INFO_V1(agg_min_transfunc);
PG_FUNCTION_INFO_V1(agg_max_transfunc);
PG_FUNCTION_INFO_V1(combine_set_mult_min);
PG_FUNCTION_INFO_V1(combine_set_mult_max);
PG_FUNCTION_INFO_V1(agg_min_set_transfunc);
PG_FUNCTION_INFO_V1(agg_max_set_transfunc);
PG_FUNCTION_INFO_V1(agg_min_max_set_finalfunc);

// count -- assumes mult is RangeType.. easy fix if not
PG_FUNCTION_INFO_V1(agg_count_transfunc);

// avg- uses agg_sum_set_transfunc as transition function
PG_FUNCTION_INFO_V1(agg_avg_range_transfunc);
PG_FUNCTION_INFO_V1(agg_avg_range_finalfunc);


// easy change for future implementation. currently only affects lift funciton
#define PRIMARY_DATA_TYPE "int4range"

/// check for NULLS parameters. Different from empty range check
// returns the parameter that is not null
#define CHECK_BINARY_PGARG_NULL_ARGS()                          \
    do {                                                        \
        if (PG_ARGISNULL(0) && PG_ARGISNULL(1))                 \
            PG_RETURN_NULL();                                   \
        else if (PG_ARGISNULL(0))                               \
            PG_RETURN_DATUM(PG_GETARG_DATUM(1));                \
        else if (PG_ARGISNULL(1))                               \
            PG_RETURN_DATUM(PG_GETARG_DATUM(0));                \
    } while (0)

// check for NULL param on either PGARG(0) OR PGARG(1)
#define CHECK_BINARY_PGARG_NULL_OR()                            \
    do {                                                        \
        if (PG_ARGISNULL(0) || PG_ARGISNULL(1))                 \
            PG_RETURN_NULL();                                   \
    } while (0)

/* takes in 2 RangeType parameters, and returns a single RangeType with provided operator result */
#define DEFINE_RANGE_ARITHMETIC_FUNC(func_name, internal_func)      \
Datum func_name(PG_FUNCTION_ARGS)                                   \
{                                                                   \
    RangeType *r1;                                                  \
    RangeType *r2;                                                  \
    RangeType *output;                                              \
    CHECK_BINARY_PGARG_NULL_ARGS();                                 \
    r1 = PG_GETARG_RANGE_P(0);                                      \
    r2 = PG_GETARG_RANGE_P(1);                                      \
    output = arithmetic_range_helper(r1, r2, internal_func);        \
    PG_RETURN_RANGE_P(output);                                      \
}

/* takes in 2 ArrayType parameters, and returns a single ArrayType with provided operator result */
#define DEFINE_SET_ARITHMETIC_FUNC(func_name, internal_func)        \
Datum func_name(PG_FUNCTION_ARGS)                                   \
{                                                                   \
    ArrayType *a1;                                                  \
    ArrayType *a2;                                                  \
    ArrayType *output;                                              \
    CHECK_BINARY_PGARG_NULL_ARGS();                                 \
    a1 = PG_GETARG_ARRAYTYPE_P(0);                                  \
    a2 = PG_GETARG_ARRAYTYPE_P(1);                                  \
    output = arithmetic_set_helper(a1, a2, internal_func);          \
    PG_RETURN_ARRAYTYPE_P(output);                                  \
}

/* takes in 2 RangeType parameters, and returns a 3VL boolean after comparison*/
#define DEFINE_RANGE_LOGICAL_FUNC(func_name, internal_func)         \
Datum func_name(PG_FUNCTION_ARGS)                                   \
{                                                                   \
    RangeType *r1;                                                  \
    RangeType *r2;                                                  \
    int rv;                                                         \
    CHECK_BINARY_PGARG_NULL_ARGS();                                 \
    r1 = PG_GETARG_RANGE_P(0);                                  \
    r2 = PG_GETARG_RANGE_P(1);                                  \
    rv = logical_range_helper(r1, r2, internal_func);               \
    if (rv == -1){                                                  \
        PG_RETURN_NULL();                                           \
    }                                                               \
    PG_RETURN_BOOL((bool)rv);                                       \
}

/* takes in 2 ArrayType parameters, and returns a 3VL boolean after comparison*/
#define DEFINE_SET_LOGICAL_FUNC(func_name, internal_func)           \
Datum func_name(PG_FUNCTION_ARGS)                                   \
{                                                                   \
    ArrayType *a1;                                                  \
    ArrayType *a2;                                                  \
    int rv;                                                         \
    CHECK_BINARY_PGARG_NULL_ARGS();                                 \
    a1 = PG_GETARG_ARRAYTYPE_P(0);                                  \
    a2 = PG_GETARG_ARRAYTYPE_P(1);                                  \
    rv = logical_set_helper(a1, a2, internal_func);                 \
    if (rv == -1){                                                  \
        PG_RETURN_NULL();                                           \
    }                                                               \
    PG_RETURN_BOOL((bool)rv);                                       \
}

/*Function declarations*/

RangeType* arithmetic_range_helper(RangeType *input1, RangeType *input2, Int4Range (*callback)(Int4Range, Int4Range));
ArrayType* arithmetic_set_helper(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet));
int logical_range_helper(RangeType *input1, RangeType *input2, int (*callback)(Int4Range, Int4Range) );
int logical_set_helper(ArrayType *input1, ArrayType *input2, int (*callback)(Int4RangeSet, Int4RangeSet) );
ArrayType* helperFunctions_helper( ArrayType *input, Int4RangeSet (*callback)() );

// for min/max agg
// can all be added to helperFunctions.h
Int4Range range_mult_combine_helper_sum(Int4Range set1, Int4Range mult, int neutralElement);
Int4RangeSet set_mult_combine_helper_sum(Int4RangeSet set1, Int4Range mult, int neutralElement);
Int4Range range_mult_combine_helper(Int4Range range, Int4Range mult, int neutralElement);
Int4RangeSet set_mult_combine_helper(Int4RangeSet set1, Int4Range mult, int neutralElement);

/////////////////////
    // Arithmetic
/////////////////////
DEFINE_RANGE_ARITHMETIC_FUNC(range_add, range_add_internal)
DEFINE_RANGE_ARITHMETIC_FUNC(range_subtract, range_subtract_internal)
DEFINE_RANGE_ARITHMETIC_FUNC(range_multiply, range_multiply_internal)
DEFINE_RANGE_ARITHMETIC_FUNC(range_divide, range_divide_internal)

DEFINE_SET_ARITHMETIC_FUNC(set_add, range_set_add_internal)
DEFINE_SET_ARITHMETIC_FUNC(set_subtract, range_set_subtract_internal)
DEFINE_SET_ARITHMETIC_FUNC(set_multiply, range_set_multiply_internal)
DEFINE_SET_ARITHMETIC_FUNC(set_divide, range_set_divide_internal)

/////////////////////
    // Comparison
/////////////////////
DEFINE_RANGE_LOGICAL_FUNC(range_gt, range_greater_than)
DEFINE_RANGE_LOGICAL_FUNC(range_gte, range_greater_than_equal)
DEFINE_RANGE_LOGICAL_FUNC(range_lt, range_less_than)
DEFINE_RANGE_LOGICAL_FUNC(range_lte, range_less_than_equal)
DEFINE_RANGE_LOGICAL_FUNC(range_eq, range_equal_internal)

DEFINE_SET_LOGICAL_FUNC(set_gt, set_greater_than)
DEFINE_SET_LOGICAL_FUNC(set_gte, set_greater_than_equal)
DEFINE_SET_LOGICAL_FUNC(set_lt, set_less_than)
DEFINE_SET_LOGICAL_FUNC(set_lte, set_less_than_equal)
DEFINE_SET_LOGICAL_FUNC(set_eq, set_equal_internal)

/////////////////////
 // Helper Functions
/////////////////////

/* lift expects 1 parameter x for example and returns a valid int4range [x, x+1) */
// Lift an Integer x into a RangeType [x, x+1)
Datum
lift_scalar(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    Oid rangeTypeOID;
    TypeCacheEntry *typcache;
    rangeTypeOID = TypenameGetTypid(PRIMARY_DATA_TYPE);
    typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    
    int unlifted;
    unlifted = PG_GETARG_INT32(0);
    
    Int4Range result;
    result = lift_scalar_local(unlifted);
    
    RangeBound lb, ub;
    lb = make_range_bound(result.lower, true, true);
    ub = make_range_bound(result.upper, false, false);
        
    RangeType *output;
    output = make_range(typcache, &lb, &ub, false, NULL);

    PG_RETURN_RANGE_P(output);
}

// FIXME- fix the local code for this. need to account for NULL. should be simple fix
// also figure out of can make a single helperFunction_helper that takes in optinal parameters
Datum
set_reduce_size(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)){
        PG_RETURN_NULL();
    }

    ArrayType *inputArray;
    int32 numRangesKeep;
    inputArray = PG_GETARG_ARRAYTYPE_P(0);
    numRangesKeep = PG_GETARG_INT32(1);

    // assign typcache based on RangeType input
    Oid rangeTypeOID;
    TypeCacheEntry *typcache;
    rangeTypeOID = ARR_ELEMTYPE(inputArray);
    typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

    Int4RangeSet set1;
    set1 = deserialize_ArrayType(inputArray, typcache);
    
    // return NULL sorted range == NULL. 
    if (set1.count == 0){
        PG_RETURN_NULL();
    }

    // reduce the set to numRangesKeep
    Int4RangeSet result;
    result = reduceSize(set1, numRangesKeep);

    // the reduced size should always be less than equal to numRangesKeep
    if (result.count < numRangesKeep) {
        ereport(ERROR,
            (errcode(ERRCODE_DATA_CORRUPTED),
            errmsg("result.count < numRangesKeep when reducing. Impossible result")));
    }

    ArrayType *output;
    output = serialize_ArrayType(result, typcache);
    
    pfree(set1.ranges);
    pfree(result.ranges);

    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
set_sort(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    ArrayType *inputArray;
    inputArray = PG_GETARG_ARRAYTYPE_P(0);

    ArrayType *output;
    output = helperFunctions_helper(inputArray, sort);

    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
set_normalize(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    ArrayType *inputArray;
    inputArray = PG_GETARG_ARRAYTYPE_P(0);

    ArrayType *output;
    output = helperFunctions_helper(inputArray, normalize);

    PG_RETURN_ARRAYTYPE_P(output);
}

/*
Takes in 2 parameters: Array: Int4RangeSet, and the function ptr callback: Int4RangeSet function() 
Generally called for helper functions that modify 1 Int4RangeSet param passed in
*/
ArrayType*
helperFunctions_helper(ArrayType *input, Int4RangeSet (*callback)() )
{
    Oid rangeTypeOID;
    rangeTypeOID = ARR_ELEMTYPE(input);
    TypeCacheEntry *typcache;
    typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

    Int4RangeSet set1;
    set1 = deserialize_ArrayType(input, typcache);

    Int4RangeSet result;
    result = callback(set1);
    
    // convert result back to native PG representation
    ArrayType *output;
    output = serialize_ArrayType(result, typcache);
    
    pfree(set1.ranges);
    pfree(result.ranges);

    return output;
}

/*
Generic Helper for arithmetic operations on RangeTypes.
Deserializes data, performs operation on data, serializes it back to native PG RangeType.
* Parameters(3): 
    -input1 RangeType: Int4Range, NON-NULL
    -input2 RangeType: Int4Range, NON-NULL
    -function ptr callback: Int4Range function()   
* Return(1):
    -RangeType result
*/
RangeType*
arithmetic_range_helper(RangeType *input1, RangeType *input2, Int4Range (*callback)(Int4Range, Int4Range))
{   
    // assign typcache based on RangeType input
    Oid rangeTypeOID1;
    Oid rangeTypeOID2;
    TypeCacheEntry *typcache;
    Int4Range range1; 
    Int4Range range2;
    Int4Range result;
    RangeType *output;

    rangeTypeOID1 = RangeTypeGetOid(input1);
    rangeTypeOID2 = RangeTypeGetOid(input2);
    typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);

    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    range1 = deserialize_RangeType(input1, typcache);
    range2 = deserialize_RangeType(input2, typcache);
    
    // safety check. Should not be necessary bc postgres enforces this already
    if(!validRange(range1) || !validRange(range2)) {
        return make_empty_range(typcache);;
    }

    // implemented C function
    result = callback(range1, range2);

    // convert result into RangeType
    output = serialize_RangeType(result, typcache);
    return output;
}

/*
Generic Helper for arithmetic operations on ArrayType of RangeTypes.
Deserializes data, performs operation on data, serializes it back to native PG type.
* Parameters(3): 
    -input1 ArrayType: Int4RangeSet, NON-NULL
    -input2 ArrayType: Int4RangeSet, NON-NULL
    -function ptr callback: Int4RangeSet function()   
* Return(1):
    -ArrayType result
*/
ArrayType*
arithmetic_set_helper(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet))
{   
    Oid rangeTypeOID1;
    Oid rangeTypeOID2;
    TypeCacheEntry *typcache;
    Int4RangeSet set1; 
    Int4RangeSet set2;
    Int4RangeSet result;
    ArrayType *output;
    
    // assign typcache based on RangeType input
    rangeTypeOID1 = ARR_ELEMTYPE(input1);
    rangeTypeOID2 = ARR_ELEMTYPE(input2);
    typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);
    
    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    // convert native PG representaion to our local representation
    set1 = deserialize_ArrayType(input1, typcache);
    set2 = deserialize_ArrayType(input2, typcache);

    if(set1.containsNull && set1.count == 1) {
        output = serialize_ArrayType(set2, typcache);
        return output;
    }
    else if(set2.containsNull && set2.count == 1) {
        output = serialize_ArrayType(set1, typcache);
        return output;
    }

    // callback function in this case is an arithmetic function with params: (Int4RangeSet a, Int4RangeSet b)
    result = callback(set1, set2);
    
    // convert result back to native PG representation
    output = serialize_ArrayType(result, typcache);

    // clean local representation
    pfree(set1.ranges);
    pfree(set2.ranges);
    pfree(result.ranges);

    return output;
}

/*
Generic Helper for logical operations on ArrayTypes of RangeTypes.
Deserializes data, performs operation on data, returns int (3VL boolean) result
* Parameters(3): 
    -input1 RangeType: Int4Range, NON-NULL
    -input2 RangeType: Int4Range, NON-NULL
    -function ptr callback: Int4Range function()   
* Return(1):
    -int result (3VL)
*/
int 
logical_range_helper(RangeType *input1, RangeType *input2, int (*callback)(Int4Range, Int4Range) )
{   
    Int4Range range1;
    Int4Range range2;
    TypeCacheEntry *typcache;
    int result;

    typcache = lookup_type_cache(input1->rangetypid, TYPECACHE_RANGE_INFO);
    if (input1->rangetypid != input2->rangetypid) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    // deserialize, operate, return integer
    range1 = deserialize_RangeType(input1, typcache);
    range2 = deserialize_RangeType(input2, typcache);
    result = callback(range1, range2);

    return result;
}

/*
Generic Helper for logical operations on ArrayTypes of RangeTypes.
Deserializes data, performs operation on data, returns int (3VL boolean) result
* Parameters(3): 
    -input1 RangeType: Int4Range, NON-NULL
    -input2 RangeType: Int4Range, NON-NULL
    -function ptr callback: Int4Range function()   
* Return(1):
    -int result (3VL)
*/
int 
logical_set_helper(ArrayType *input1, ArrayType *input2, int (*callback)(Int4RangeSet, Int4RangeSet) )
{   
    // assign typcache based on RangeType input
    Oid rangeTypeOID1;
    Oid rangeTypeOID2;
    TypeCacheEntry *typcache;
    Int4RangeSet set1;
    Int4RangeSet set2;
    int result;

    rangeTypeOID1 = ARR_ELEMTYPE(input1);
    rangeTypeOID2 = ARR_ELEMTYPE(input2);
    typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);
    
    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    // deserialize, operate, return integer
    set1 = deserialize_ArrayType(input1, typcache);
    set2 = deserialize_ArrayType(input2, typcache);
    result = callback(set1, set2);

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// TESTING
///////////////////////////////////////////////////////////////////////////////////////////////

ArrayType*
normalizeRange(ArrayType *input1) {
    Oid elemTypeOID;
    TypeCacheEntry *typcache;

    elemTypeOID = input1->elemtype;
    typcache = lookup_type_cache(elemTypeOID, TYPECACHE_RANGE_INFO);
    
    // deconstruct array, create our representation of I4R, call function and get 'normalized' result
    Datum *elems1;
    bool *nulls1;
    int n1;
    deconstruct_array(input1, elemTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems1, &nulls1, &n1);

    // Our representation of I4R. Should be freed after normalizing/reducing to potentially smaller range
    Int4RangeSet set1;
    set1.containsNull = false;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    
    int currIdx = 0;
    int i;
    for(i=0; i < n1; i++){
        
        // the RangeType at this index is NULL
        // only insert a NULL entry ONCE into the currIdx
        if (nulls1[i] && !set1.containsNull) {
            set1.ranges[currIdx].lower = 0;
            set1.ranges[currIdx].upper = 0; // maybe remove?

            set1.ranges[currIdx].isNull = true;
            set1.containsNull = true;
            currIdx++;
        }
        // otherwise Extract RangeType elements
        else {
            RangeType *curr;
            RangeBound l1, u1;
            bool isEmpty1;
            
            curr = DatumGetRangeTypeP(elems1[i]);
            range_deserialize(typcache, curr, &l1, &u1, &isEmpty1);

            // if the RangeType is not empty, append to 
            if (!isEmpty1) {
                set1.ranges[currIdx].lower = DatumGetInt32(l1.val);
                set1.ranges[currIdx].upper = DatumGetInt32(u1.val);
                set1.ranges[currIdx].isNull = false;
                currIdx++;
            }
        }
    }
    set1.count = currIdx;
    
    Int4Range *temp;
    // return empty array if there is no result
    if (set1.count == 0) {
        pfree(set1.ranges);
        return construct_empty_array(elemTypeOID);
    }
    // change size of working set after removing over allocated arr
    else {
        temp = repalloc(set1.ranges, sizeof(Int4Range) * set1.count);
        if (temp != NULL) set1.ranges = temp;
    }

    // Remove all possible overlap.
    Int4RangeSet rv;
    rv = normalize(set1);

    Datum *datums;
    bool  *nulls;
    datums = palloc(sizeof(Datum) * rv.count);
    nulls  = palloc(sizeof(bool) * rv.count);
    
    // convert self defined type into valid Postgres Type
    for(i=0; i<rv.count; i++){
        // trigger NULL index properly
        if (rv.ranges[i].isNull) {
            nulls[i] = true;
            datums[i] = (Datum) 0;
            continue;
        }
        
        nulls[i] = false;
        
        RangeBound lowerRv, upperRv;
        lowerRv.val = Int32GetDatum(rv.ranges[i].lower);
        lowerRv.inclusive = true;
        lowerRv.infinite = false;
        lowerRv.lower = true;

        upperRv.val = Int32GetDatum(rv.ranges[i].upper);
        upperRv.inclusive = false;
        upperRv.infinite = false;
        upperRv.lower = false;

        RangeType *r = make_range(typcache, &lowerRv, &upperRv, false, NULL);
        datums[i] = RangeTypePGetDatum(r);
    }

    // Convert array of Datums into an ArrayType
    int ndim;
    int dims[1];
    int lbs[1];
    
    ndim = 1;
    dims[0] = rv.count;
    lbs[0] = 1;

    ArrayType *resultsArrOut = construct_md_array(datums, nulls, ndim, dims, lbs, elemTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign);
    
    pfree(elems1);
    pfree(nulls1);
    pfree(set1.ranges);
    pfree(rv.ranges);

    return resultsArrOut;
}

/*
// Returns naturalElement Set if multiplicity is 0, otherwise original Set. 
// naturalElement Set does not affect min/max calculation
*/
// FIXME will need to change the type of neutral element depending on what datatype the user is using
Int4Range
range_mult_combine_helper_sum(Int4Range set1, Int4Range mult, int neutralElement)
{
    // return neutral so doesn't affect the aggregate
    if(mult.lower == 0) {
        Int4Range result;
        result.isNull = false;
    
        // have to adjust UB + 2 or LB -2 based on if pos or neg
        if (neutralElement <= 0) {
            result.lower = 0;      //temp change to resolve crashing   
            result.upper = 1 ;
        }
        else {
            result.lower = neutralElement;      //temp change to resolve crashing   
            result.upper = neutralElement + 1;
        }
        return result;
    }

    return set1;
}

/*
// Returns naturalElement Set if multiplicity is 0, otherwise original Set. 
// naturalElement Set does not affect min/max calculation
*/
// FIXME will need to change the type of neutral element depending on what datatype the user is using
Int4RangeSet
set_mult_combine_helper_sum(Int4RangeSet set1, Int4Range mult, int neutralElement)
{
    // return neutral so doesn't affect the aggregate
    if(mult.lower == 0) {
        Int4RangeSet result;
        result.count = 1;
        result.containsNull = false;
        result.ranges = palloc(sizeof(Int4Range));
        result.ranges[0].isNull = false;
    
        // have to adjust UB + 2 or LB -2 based on if pos or neg
        if (neutralElement <= 0) {
            result.ranges[0].lower = 0;      //temp change to resolve crashing   
            result.ranges[0].upper = 1;
        }
        else {
            result.ranges[0].lower = neutralElement;      //temp change to resolve crashing   
            result.ranges[0].upper = neutralElement + 1;
        }
        return result;
    }

    return set1;
}

/*
// To be called inside a MAX aggregation call. This multiplies the Set and multiplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Parameter: ArrayType (data col), RangeType (multiplicity)
// Returns: a ArrayType Datum as argument to MAX()
*/
Datum
combine_range_mult_sum(PG_FUNCTION_ARGS) 
{
    // inputs/ outputs
    RangeType *input, *mult_input, *output;
    
    // working type
    Int4Range input_i4r, result_i4r, mult_i4r;

    int neutral_element;
    TypeCacheEntry *typcache, *typcacheMult;
    
    CHECK_BINARY_PGARG_NULL_OR();
    
    input = PG_GETARG_RANGE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);

    typcache = lookup_type_cache(input->rangetypid, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);

    // hardcoded //FIXME
    neutral_element = 0;

    // deserialize, operate on, serialize, return
    input_i4r = deserialize_RangeType(input, typcache);
    mult_i4r = deserialize_RangeType(mult_input, typcacheMult);

    result_i4r = range_mult_combine_helper_sum(input_i4r, mult_i4r, neutral_element);
    output = serialize_RangeType(result_i4r, typcache);

    PG_RETURN_ARRAYTYPE_P(output);
}

/*
// To be called inside a MAX aggregation call. This multiplies the Set and multiplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Parameter: ArrayType (data col), RangeType (multiplicity)
// Returns: a ArrayType Datum as argument to MAX()
*/
Datum
combine_set_mult_sum(PG_FUNCTION_ARGS) 
{
    // inputs/ outputs
    ArrayType *set_input, *output;
    RangeType *mult_input;
    
    // working type
    Int4Range mult;
    Int4RangeSet set1, result;

    int neutral_element;
    TypeCacheEntry *typcacheSet, *typcacheMult;
    
    CHECK_BINARY_PGARG_NULL_OR();
    
    set_input = PG_GETARG_ARRAYTYPE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);

    typcacheSet = lookup_type_cache(set_input->elemtype, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);

    // check for empty array
    if (ArrayGetNItems(ARR_NDIM(set_input), ARR_DIMS(set_input)) == 0) {
        PG_RETURN_NULL();
    }
    // check for empty mult
    if RangeIsEmpty(mult_input){
        PG_RETURN_NULL();
    }

    // hardcoded //FIXME
    neutral_element = 0;

    // deserialize, operate on, serialize, return
    set1 = deserialize_ArrayType(set_input, typcacheSet);
    mult = deserialize_RangeType(mult_input, typcacheMult);

    // check for mult LB = 0
    if (mult.lower == 0){
        pfree(set1.ranges);
        PG_RETURN_NULL();
    }

    result = set_mult_combine_helper(set1, mult, neutral_element);
    output = serialize_ArrayType(result, typcacheSet);
    
    // clean
    pfree(set1.ranges);
    if (result.ranges != set1.ranges) {
        pfree(result.ranges);
    }

    PG_RETURN_ARRAYTYPE_P(output);
}

// agg over empty == 0,0
/* arbitrary trigger size. doesnt use for now 
    first parameter is the state
    second parameter is the current colA (I4RSet)
    third parameter is the multiplicity
 */
Datum
agg_sum_transfunc(PG_FUNCTION_ARGS)
{
    RangeType *state, *input, *result;
    TypeCacheEntry *typcache;
    
    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)){
        if (PG_ARGISNULL(1)){
            PG_RETURN_NULL();
        }
        // othrwise value becomes the state
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(1));
    }

    // NULL input: return current state unchanged
    if(PG_ARGISNULL(1)) {
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(0));
    }
    
    // get arguments, call helper to get result, check to normalize after
    state = PG_GETARG_RANGE_P(0);
    input = PG_GETARG_RANGE_P(1);

    result = arithmetic_range_helper(state, input, range_add_internal);

    PG_RETURN_ARRAYTYPE_P(result);
}


/*
State Transition function for max aggregate
Returns the minimum LB and UB of all ranges in column.
Simply deserializes data, operates on it, and serializes 
    State = Int4Range = [a,b)
    Input = Int4Range = [c,d)
    Return RangeType: [min(a,c), min(b,d))
*/
Datum
agg_min_transfunc(PG_FUNCTION_ARGS)
{
    Int4Range state_i4r, input_i4r, result_i4r;
    RangeType *state, *input, *result;
    TypeCacheEntry *typcache;

    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)) {
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        }
        // othrwise value becomes the state
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(1));
    }
    
    // NULL input: return current state unchanged
    if (PG_ARGISNULL(1)) {
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(0));
    }

    // compare existing min/state to the current input
    state = PG_GETARG_RANGE_P(0);
    input = PG_GETARG_RANGE_P(1);

    // return non empty
    if (RangeIsEmpty(state)) {
        PG_RETURN_POINTER(input);
    }
    if (RangeIsEmpty(input)) {
        PG_RETURN_POINTER(state);
    }
    
    typcache = lookup_type_cache(state->rangetypid, TYPECACHE_RANGE_INFO);
    
    // deserialize, compare, serialize, return
    state_i4r = deserialize_RangeType(state, typcache);
    input_i4r = deserialize_RangeType(input, typcache);
    result_i4r = min_range(state_i4r, input_i4r);
    result = serialize_RangeType(result_i4r, typcache);

    PG_RETURN_POINTER(result);
}

/*
State Transition function for max aggregate
Returns the maximum LB and UB of all ranges in column.
Simply deserializes data, operates on it, and serializes 
    State = Int4Range = [a,b)
    Input = Int4Range = [c,d)
    Return RangeType: [max(a,c), max(b,d))
*/
Datum
agg_max_transfunc(PG_FUNCTION_ARGS)
{
    Int4Range state_i4r, input_i4r, result_i4r;
    RangeType *state, *input, *result;
    TypeCacheEntry *typcache;

    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)) {
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        }

        // othrwise value becomes the state
        PG_RETURN_POINTER(PG_GETARG_RANGE_P(1));
    }
    // NULL input: return current state unchanged
    if (PG_ARGISNULL(1)) {
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(0));
    }
    
    state = PG_GETARG_RANGE_P(0);
    input = PG_GETARG_RANGE_P(1);
    
    // return non empty
    if (RangeIsEmpty(state)) {
        PG_RETURN_POINTER(input);
    }
    if (RangeIsEmpty(input)) {
        PG_RETURN_POINTER(state);
    }
    
    typcache = lookup_type_cache(input->rangetypid, TYPECACHE_RANGE_INFO);
    
    // deserialize, compare, serialize, return
    state_i4r = deserialize_RangeType(state, typcache);
    input_i4r = deserialize_RangeType(input, typcache);
    result_i4r = max_range(state_i4r, input_i4r);
    result = serialize_RangeType(result_i4r, typcache);

    PG_RETURN_POINTER(result);
}

/*
// Returns naturalElement Range if multiplicity is 0, otherwise original range. 
// naturalElement Range does not affect min/max calculation
*/
// FIXME will need to change the type of neutral element depending on what datatype the user is using
Int4Range
range_mult_combine_helper(Int4Range range, Int4Range mult, int neutralElement)
{
    // return neutral so doesn't affect the aggregate
    if(mult.lower == 0) {
        Int4Range result;
        result.isNull = true; //auto false, not using NULLs

        // have to adjust UB + 2 or LB -2 based on if pos or neg
        if (neutralElement <= 0) {
            result.lower = neutralElement;      //temp change to resolve crashing   
            result.upper = neutralElement + 2;
        }
        else {
            result.lower = neutralElement-2;      //temp change to resolve crashing   
            result.upper = neutralElement;
        }
        return result;
    }

    return range;
}

/*
// Returns naturalElement Set if multiplicity is 0, otherwise original Set. 
// naturalElement Set does not affect min/max calculation
*/
// FIXME will need to change the type of neutral element depending on what datatype the user is using
Int4RangeSet
set_mult_combine_helper(Int4RangeSet set1, Int4Range mult, int neutralElement)
{
    // return neutral so doesn't affect the aggregate
    if(mult.lower == 0) {
        Int4RangeSet result;
        result.count = 1;
        result.containsNull = false;
        result.ranges = palloc(sizeof(Int4Range));
        result.ranges[0].isNull = false;
    
        // have to adjust UB + 2 or LB -2 based on if pos or neg
        if (neutralElement <= 0) {
            result.ranges[0].lower = neutralElement + 1;      //temp change to resolve crashing   
            result.ranges[0].upper = neutralElement + 10;
        }
        else {
            result.ranges[0].lower = neutralElement-10;      //temp change to resolve crashing   
            result.ranges[0].upper = neutralElement -1;
        }
        return result;
    }

    return set1;
}


/*
// To be called inside a MIN aggregation call. This multiplies the range and multuplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Returns: a RangeType Datum as argument to MIN()
*/
Datum
combine_range_mult_min(PG_FUNCTION_ARGS) 
{
    RangeType *range_input, *mult_input, *output;
    Int4Range range, mult, result;
    int neutral_element;
    TypeCacheEntry *typcacheRange, *typcacheMult;
    
    // need better handling of mult null
    CHECK_BINARY_PGARG_NULL_OR();

    range_input = PG_GETARG_RANGE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);
    typcacheRange = lookup_type_cache(range_input->rangetypid, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);
    
    // hardcoded //FIXME
    neutral_element = INT_MAX;

    range = deserialize_RangeType(range_input, typcacheRange);
    mult = deserialize_RangeType(mult_input, typcacheMult);

    result = range_mult_combine_helper(range, mult, neutral_element);    
    output = serialize_RangeType(result, typcacheRange);

    PG_RETURN_RANGE_P(output);
}

/*
// To be called inside a MAX aggregation call. This multiplies the range and multuplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Returns: a RangeType Datum as argument to MAX()
*/
Datum
combine_range_mult_max(PG_FUNCTION_ARGS) 
{
    RangeType *range_input, *mult_input, *output;
    Int4Range range, mult, result;
    int neutral_element;
    TypeCacheEntry *typcacheRange, *typcacheMult;
    
    CHECK_BINARY_PGARG_NULL_OR();
    
    range_input = PG_GETARG_RANGE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);
    typcacheRange = lookup_type_cache(range_input->rangetypid, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);

    // hardcoded //FIXME
    neutral_element = INT_MIN;

    range = deserialize_RangeType(range_input, typcacheRange);
    mult = deserialize_RangeType(mult_input, typcacheMult);

    result = range_mult_combine_helper(range, mult, neutral_element);
    output = serialize_RangeType(result, typcacheRange);

    PG_RETURN_RANGE_P(output);
}

/*
// To be called inside a MIN aggregation call. This multiplies the Set and multiplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Parameter: ArrayType (data col), RangeType (multiplicity)
// Returns: a ArrayType Datum as argument to MIN()
*/
Datum
combine_set_mult_min(PG_FUNCTION_ARGS) 
{
   // inputs/ outputs
    ArrayType *set_input, *output;
    RangeType *mult_input;
    
    // working type
    Int4Range mult;
    Int4RangeSet set1, result;

    int neutral_element;
    TypeCacheEntry *typcacheSet, *typcacheMult;
    
    CHECK_BINARY_PGARG_NULL_OR();
    
    set_input = PG_GETARG_ARRAYTYPE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);

    typcacheSet = lookup_type_cache(set_input->elemtype, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);

    // hardcoded //FIXME
    neutral_element = INT_MAX;

    // deserialize, operate on, serialize, return
    set1 = deserialize_ArrayType(set_input, typcacheSet);
    mult = deserialize_RangeType(mult_input, typcacheMult);

    result = set_mult_combine_helper(set1, mult, neutral_element);
    output = serialize_ArrayType(result, typcacheSet);

    PG_RETURN_ARRAYTYPE_P(output);
}

/*
// To be called inside a MAX aggregation call. This multiplies the Set and multiplicity together.
// neutral_element is the only difference between min/max implementation. This value is HARDCODED //FIXME
// Parameter: ArrayType (data col), RangeType (multiplicity)
// Returns: a ArrayType Datum as argument to MAX()
*/
Datum
combine_set_mult_max(PG_FUNCTION_ARGS) 
{
    // inputs/ outputs
    ArrayType *set_input, *output;
    RangeType *mult_input;
    
    // working type
    Int4Range mult;
    Int4RangeSet set1, result;

    int neutral_element;
    TypeCacheEntry *typcacheSet, *typcacheMult;
    
    CHECK_BINARY_PGARG_NULL_OR();
    
    set_input = PG_GETARG_ARRAYTYPE_P(0);
    mult_input = PG_GETARG_RANGE_P(1);

    typcacheSet = lookup_type_cache(set_input->elemtype, TYPECACHE_RANGE_INFO);
    typcacheMult = lookup_type_cache(mult_input->rangetypid, TYPECACHE_RANGE_INFO);

    // hardcoded //FIXME
    neutral_element = INT_MIN;

    // deserialize, operate on, serialize, return
    set1 = deserialize_ArrayType(set_input, typcacheSet);
    mult = deserialize_RangeType(mult_input, typcacheMult);

    result = set_mult_combine_helper(set1, mult, neutral_element);
    output = serialize_ArrayType(result, typcacheSet);

    PG_RETURN_ARRAYTYPE_P(output);
}

/*
// State Transition function for min aggregate
// Returns the minimum LB and UB of all ranges in column.
// Simply deserializes data, operates on it, and serializes back to ArrayType
//     State = ArrayType = {[a,b) ...}       //(implicit) 
//     Input = ArrayType = [c,d) ... }
// Return ArrayType: {[min(a,c), min(b,d)) for all ranges}
*/
Datum
agg_min_set_transfunc(PG_FUNCTION_ARGS)
{
    Int4RangeSet state_i4r, input_i4r, n_state_i4r, n_input_i4r, result_i4r;
    ArrayType *state, *input, *result;
    TypeCacheEntry *typcache;

    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)){
        if (PG_ARGISNULL(1)){
            PG_RETURN_NULL();
        }
        // othrwise value becomes the state
        PG_RETURN_ARRAYTYPE_P(PG_GETARG_ARRAYTYPE_P(1));
    }

    // NULL input: return current state unchanged
    if(PG_ARGISNULL(1)) {
        PG_RETURN_ARRAYTYPE_P(PG_GETARG_ARRAYTYPE_P(0));
    }

    // compare existing min/state to the current input
    state = PG_GETARG_ARRAYTYPE_P(0);
    input = PG_GETARG_ARRAYTYPE_P(1);

    // handle empty array, or array with just null maybe
    // if (array_contains_nulls())

    typcache = lookup_type_cache(state->elemtype, TYPECACHE_RANGE_INFO);

    state_i4r = deserialize_ArrayType(state, typcache);
    input_i4r = deserialize_ArrayType(input, typcache);

    n_state_i4r = normalize(state_i4r);
    n_input_i4r = normalize(input_i4r);

    result_i4r = min_rangeSet(n_state_i4r, n_input_i4r);
    result = serialize_ArrayType(result_i4r, typcache);

    PG_RETURN_ARRAYTYPE_P(result);
}

/*
// State Transition function for max aggregate
// Returns the minimum LB and UB of all ranges in column.
// Simply deserializes data, operates on it, and serializes back to ArrayType
//     State = ArrayType = {[a,b) ...}       //(implicit) 
//     Input = ArrayType = [c,d) ... }
// Return ArrayType: {[max(a,c), max(b,d)) for all ranges}
*/
Datum
agg_max_set_transfunc(PG_FUNCTION_ARGS)
{
    Int4RangeSet state_i4r, input_i4r, n_state_i4r, n_input_i4r, result_i4r;
    ArrayType *state, *input, *result;
    TypeCacheEntry *typcache;

    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)){
        if (PG_ARGISNULL(1)){
            PG_RETURN_NULL();
        }
        // othrwise value becomes the state
        PG_RETURN_ARRAYTYPE_P(PG_GETARG_ARRAYTYPE_P(1));
    }

    // NULL input: return current state unchanged
    if(PG_ARGISNULL(1)) {
        PG_RETURN_ARRAYTYPE_P(PG_GETARG_ARRAYTYPE_P(0));
    }

    // compare existing min/state to the current input
    state = PG_GETARG_ARRAYTYPE_P(0);
    input = PG_GETARG_ARRAYTYPE_P(1);

    // handle empty array, or array with just null maybe
    // if (array_contains_nulls())

    typcache = lookup_type_cache(state->elemtype, TYPECACHE_RANGE_INFO);

    state_i4r = deserialize_ArrayType(state, typcache);
    input_i4r = deserialize_ArrayType(input, typcache);

    n_state_i4r = normalize(state_i4r);
    n_input_i4r = normalize(input_i4r);
    
    result_i4r = max_rangeSet(n_state_i4r, n_input_i4r);
    result = serialize_ArrayType(result_i4r, typcache);
    
    PG_RETURN_ARRAYTYPE_P(result);
}

// Simply just normalizes the result. Compressing any ranges if possible
Datum 
agg_min_max_set_finalfunc(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    ArrayType *inputArray;
    inputArray = PG_GETARG_ARRAYTYPE_P(0);

    ArrayType *output;
    output = helperFunctions_helper(inputArray, normalize);

    PG_RETURN_ARRAYTYPE_P(output);
}

/*
    Only necessary for multiplicty. Takes in mult as param and counts the total number of possible ranges
*/
Datum
agg_count_transfunc(PG_FUNCTION_ARGS)
{
    RangeType *state, *input, *result;
    TypeCacheEntry *typcache;
    
    // first call: use the first input as initial state, or non null
    if (PG_ARGISNULL(0)){
        if (PG_ARGISNULL(1)){
            PG_RETURN_NULL();
        }
        // othrwise value becomes the state
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(1));
    }

    // NULL input: return current state unchanged
    if(PG_ARGISNULL(1)) {
        PG_RETURN_RANGE_P(PG_GETARG_RANGE_P(0));
    }
    
    // get arguments, call helper to get result, check to normalize after
    state = PG_GETARG_RANGE_P(0);
    input = PG_GETARG_RANGE_P(1);

    result = arithmetic_range_helper(state, input, range_add_internal);

    PG_RETURN_ARRAYTYPE_P(result);
}

Datum
agg_sum_set_transfunc(PG_FUNCTION_ARGS)
{
    MemoryContext aggcontext, oldcontext;
    SumAggState *state;
    ArrayType *currSet;
    TypeCacheEntry *typcache;
    Int4RangeSet inputSet, combined, reduced;
    
    if (!AggCheckCallContext(fcinfo, &aggcontext))
        elog(ERROR, "agg_sum_set_transfunc called in non-aggregate context");
    
    // first call, state is NULL
    if (PG_ARGISNULL(0)) {

        // check for NULL input param, or empty
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        }
        
        currSet = PG_GETARG_ARRAYTYPE_P(1);
        typcache = lookup_type_cache(ARR_ELEMTYPE(currSet), TYPECACHE_RANGE_INFO);
        
        // empty set, continue on until non empty
        if (ArrayGetNItems(ARR_NDIM(currSet), ARR_DIMS(currSet)) == 0) {
            PG_RETURN_NULL();
        }

        // switch to aggregate memory context for persistent allocations
        oldcontext = MemoryContextSwitchTo(aggcontext);
        
        // internal state init
        state = (SumAggState *) palloc0(sizeof(SumAggState));
        state->ranges = deserialize_ArrayType(currSet, typcache);
        state->resizeTrigger = PG_GETARG_INT32(2);
        state->sizeLimit = PG_GETARG_INT32(3);
        
        // need to return to callers context
        MemoryContextSwitchTo(oldcontext);
        
        PG_RETURN_POINTER(state);
    }
    
    // otherwise merge into existing state
    state = (SumAggState *) PG_GETARG_POINTER(0);

    if (!PG_ARGISNULL(1)) {
        currSet = PG_GETARG_ARRAYTYPE_P(1);
        typcache = lookup_type_cache(ARR_ELEMTYPE(currSet), TYPECACHE_RANGE_INFO);

        // empty check
        if (ArrayGetNItems(ARR_NDIM(currSet), ARR_DIMS(currSet)) == 0) {
            PG_RETURN_POINTER(state);
        }
        
        // deserialize input in current context (freed later)
        inputSet = deserialize_ArrayType(currSet, typcache);
        
        // agg context persists data
        oldcontext = MemoryContextSwitchTo(aggcontext);
        combined = range_set_add_internal(state->ranges, inputSet);
        
        // free old ranges
        if (state->ranges.ranges != NULL) {
            pfree(state->ranges.ranges);
        }
        
        // check reduce size
        if (combined.count >= state->resizeTrigger) {
            reduced = reduceSize(combined, state->sizeLimit);
            pfree(combined.ranges);
            state->ranges = reduced;
        }
        else {
            state->ranges = combined;
        }
        
        MemoryContextSwitchTo(oldcontext);
        
        // free previous memory context
        pfree(inputSet.ranges);
    }
    
    PG_RETURN_POINTER(state);
}

/*
    Reduce one last time if needed and Convert Internal type to ArrayType Datum.
*/
Datum
agg_sum_set_finalfunc(PG_FUNCTION_ARGS)
{
    SumAggState *state;
    ArrayType *result;
    TypeCacheEntry *typcache;
    Int4RangeSet reduced;
    Oid elemTypeOID;
    
    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    
    state = (SumAggState*) PG_GETARG_POINTER(0);
    // empty state
    if (state->ranges.count == 0) {
        elemTypeOID = TypenameGetTypid("int4range");
        PG_RETURN_ARRAYTYPE_P(construct_empty_array(elemTypeOID));
    }
    
    elemTypeOID = TypenameGetTypid("int4range");
    typcache = lookup_type_cache(elemTypeOID, TYPECACHE_RANGE_INFO);
    
    // reduce final time
    if (state->ranges.count >= state->resizeTrigger) {
        reduced = reduceSize(state->ranges, state->sizeLimit);
        result = serialize_ArrayType(reduced, typcache);
        pfree(reduced.ranges);
        PG_RETURN_ARRAYTYPE_P(result);
    }
    
    result = serialize_ArrayType(state->ranges, typcache);
    PG_RETURN_ARRAYTYPE_P(result);
}
