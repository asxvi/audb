#include "postgres.h"
#include "fmgr.h"
#include "utils/rangetypes.h"   // non set functions pass RangeType params
#include "utils/array.h"        // set functions pass ArrayType params
#include "utils/typcache.h"
#include "utils/lsyscache.h"    // "Convenience routines for common queries in the system catalog cache."
#include "catalog/pg_type_d.h"  // pg_type oid macros
#include "catalog/namespace.h"
#include "utils/rangetypes.h"

#include "arithmetic.h"         // c implemented I4R functions
#include "logicalOperators.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(c_range_add);
PG_FUNCTION_INFO_V1(c_range_subtract);
PG_FUNCTION_INFO_V1(c_range_multiply);
PG_FUNCTION_INFO_V1(c_range_divide);
PG_FUNCTION_INFO_V1(c_range_set_add);
PG_FUNCTION_INFO_V1(c_range_set_subtract);
PG_FUNCTION_INFO_V1(c_range_set_multiply);
PG_FUNCTION_INFO_V1(c_range_set_divide);

PG_FUNCTION_INFO_V1(c_lt);
PG_FUNCTION_INFO_V1(c_lte);
PG_FUNCTION_INFO_V1(c_gt);
PG_FUNCTION_INFO_V1(c_gte);

PG_FUNCTION_INFO_V1(c_lift_scalar);
PG_FUNCTION_INFO_V1(c_sort);
PG_FUNCTION_INFO_V1(c_normalize);
PG_FUNCTION_INFO_V1(c_reduceSize);

PG_FUNCTION_INFO_V1(test_c_range_set_sum);
PG_FUNCTION_INFO_V1(interval_agg_transfunc);


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
#define CHECK_BINARY_PGARG_NULL_OR()                              \
    do {                                                        \
        if (PG_ARGISNULL(0) || PG_ARGISNULL(1))                 \
            PG_RETURN_NULL();                                   \
    } while (0)


// helper function declarations

RangeType* arithmetic_range_helper(RangeType *input1, RangeType *input2, Int4Range (*callback)(Int4Range, Int4Range));
ArrayType* arithmetic_set_helper(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet));
int logical_operation_helper(ArrayType *input1, ArrayType *input2, int (*callback)(Int4RangeSet, Int4RangeSet) );
ArrayType* helperFunctions_helper( ArrayType *input, Int4RangeSet (*callback)() );

// serialization and deserialization function declarations

static Int4Range deserialize_RangeType(RangeType *rng, TypeCacheEntry *typcache);
static RangeType* serialize_RangeType(Int4Range range, TypeCacheEntry *typcache);
static RangeBound make_range_bound(int32 val, bool is_lower);
static RangeBound make_closed_range_bound(int32 val, bool is_lower);
static Int4RangeSet deserialize_ArrayType(ArrayType *arr, TypeCacheEntry *typcache);
static ArrayType* serialize_ArrayType(Int4RangeSet set, TypeCacheEntry *typcache);

/*
    takes in 2 pg RangeType parameters, and returns
    a single RangeType with provided operator result
*/
Datum
c_range_add(PG_FUNCTION_ARGS)
{   
    RangeType *r1;
    RangeType *r2;
    RangeType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    r1 = PG_GETARG_RANGE_P(0);
    r2 = PG_GETARG_RANGE_P(1);
    
    output = arithmetic_range_helper(r1, r2, range_add);

    PG_RETURN_RANGE_P(output);
}

Datum
c_range_subtract(PG_FUNCTION_ARGS)
{   
    RangeType *r1;
    RangeType *r2;
    RangeType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    r1 = PG_GETARG_RANGE_P(0);
    r2 = PG_GETARG_RANGE_P(1);

    output = arithmetic_range_helper(r1, r2, range_subtract);

    PG_RETURN_RANGE_P(output);
}

Datum
c_range_multiply(PG_FUNCTION_ARGS)
{   
    RangeType *r1;
    RangeType *r2;
    RangeType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    r1 = PG_GETARG_RANGE_P(0);
    r2 = PG_GETARG_RANGE_P(1);

    output = arithmetic_range_helper(r1, r2, range_multiply);

    PG_RETURN_RANGE_P(output);
}

Datum
c_range_divide(PG_FUNCTION_ARGS)
{   
    RangeType *r1;
    RangeType *r2;
    RangeType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    r1 = PG_GETARG_RANGE_P(0);
    r2 = PG_GETARG_RANGE_P(1);

    output = arithmetic_range_helper(r1, r2, range_divide);

    PG_RETURN_RANGE_P(output);
}

Datum
c_range_set_add(PG_FUNCTION_ARGS)
{
    ArrayType *a1;
    ArrayType *a2;
    ArrayType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    output = arithmetic_set_helper(a1, a2, range_set_add);
    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
c_range_set_subtract(PG_FUNCTION_ARGS)
{
    ArrayType *a1;
    ArrayType *a2;
    ArrayType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    output = arithmetic_set_helper(a1, a2, range_set_subtract);
    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
c_range_set_multiply(PG_FUNCTION_ARGS)
{
    ArrayType *a1;
    ArrayType *a2;
    ArrayType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    output = arithmetic_set_helper(a1, a2, range_set_multiply);
    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
c_range_set_divide(PG_FUNCTION_ARGS)
{   
    ArrayType *a1;
    ArrayType *a2;
    ArrayType *output;

    CHECK_BINARY_PGARG_NULL_ARGS();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    output = arithmetic_set_helper(a1, a2, range_set_divide);
    PG_RETURN_ARRAYTYPE_P(output);
}

/////////////////////
    // Comparison
/////////////////////

Datum
c_lt(PG_FUNCTION_ARGS)
{   
    ArrayType *a1;
    ArrayType *a2;
    int rv;

    CHECK_BINARY_PGARG_NULL_OR();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    rv = logical_operation_helper(a1, a2, range_less_than);

    if (rv == -1){
        PG_RETURN_NULL();
    }

    PG_RETURN_BOOL((bool)rv);
}

Datum
c_gt(PG_FUNCTION_ARGS)
{   
    ArrayType *a1;
    ArrayType *a2;
    int rv;

    CHECK_BINARY_PGARG_NULL_OR();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);
    
    rv = logical_operation_helper(a1, a2, range_greater_than);
    
    if (rv == -1){
        PG_RETURN_NULL();
    }

    PG_RETURN_BOOL((bool)rv);
}

Datum
c_lte(PG_FUNCTION_ARGS)
{   
    ArrayType *a1;
    ArrayType *a2;
    int rv;

    CHECK_BINARY_PGARG_NULL_OR();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    rv = logical_operation_helper(a1, a2, range_less_than_equal);

    if (rv == -1){
        PG_RETURN_NULL();
    }

    PG_RETURN_BOOL((bool)rv);
}

Datum
c_gte(PG_FUNCTION_ARGS)
{   
    ArrayType *a1;
    ArrayType *a2;
    int rv;

    CHECK_BINARY_PGARG_NULL_OR();

    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);

    rv = logical_operation_helper(a1, a2, range_greater_than_equal);
    
    if (rv == -1){
        PG_RETURN_NULL();
    }

    PG_RETURN_BOOL((bool)rv);
}

/////////////////////
 // Helper Functions
/////////////////////

/* lift expects 1 parameter x for example and returns a valid int4range [x, x+1) */
// Lift an Integer x into a RangeType [x, x+1)
Datum
c_lift_scalar(PG_FUNCTION_ARGS)
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
    result = lift_scalar(unlifted);
    
    RangeBound lb, ub;
    lb = make_range_bound(result.lower, true);
    ub = make_range_bound(result.upper, false);
        
    RangeType *output;
    output = make_range(typcache, &lb, &ub, false, NULL);

    PG_RETURN_RANGE_P(output);
}

// FIXME- fix the local code for this. need to account for NULL. should be simple fix
// also figure out of can make a single helperFunction_helper that takes in optinal parameters
Datum
c_reduceSize(PG_FUNCTION_ARGS)
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
c_sort(PG_FUNCTION_ARGS)
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
c_normalize(PG_FUNCTION_ARGS)
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

    rangeTypeOID1 = RangeTypeGetOid(input1);
    rangeTypeOID2 = RangeTypeGetOid(input2);
    typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);

    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    Int4Range range1, range2;

    range1 = deserialize_RangeType(input1, typcache);
    range2 = deserialize_RangeType(input2, typcache);
    
    // safety check. Should not be necessary bc postgres enforces this already
    if(!validRange(range1) || !validRange(range2)) {
        return make_empty_range(typcache);;
    }

    // implemented C function
    Int4Range result;
    result = callback(range1, range2);

    // convert result into RangeType
    RangeType *output;
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
    // assign typcache based on RangeType input
    Oid rangeTypeOID1;
    Oid rangeTypeOID2;
    rangeTypeOID1 = ARR_ELEMTYPE(input1);
    rangeTypeOID2 = ARR_ELEMTYPE(input2);
    TypeCacheEntry *typcache;
    typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);
    
    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    // convert native PG representaion to our local representation
    Int4RangeSet set1, set2;
    set1 = deserialize_ArrayType(input1, typcache);
    set2 = deserialize_ArrayType(input2, typcache);

    // callback function in this case is an arithmetic function with params: (Int4RangeSet a, Int4RangeSet b)
    Int4RangeSet result;
    result = callback(set1, set2);
    
    // convert result back to native PG representation
    ArrayType *output;
    output = serialize_ArrayType(result, typcache);

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
logical_operation_helper(ArrayType *input1, ArrayType *input2, int (*callback)(Int4RangeSet, Int4RangeSet) )
{   
    // assign typcache based on RangeType input
    Oid rangeTypeOID1;
    Oid rangeTypeOID2;
    rangeTypeOID1 = ARR_ELEMTYPE(input1);
    rangeTypeOID2 = ARR_ELEMTYPE(input2);
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID1, TYPECACHE_RANGE_INFO);
    
    if (rangeTypeOID1 != rangeTypeOID2) {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("range type mismatch in arithmetic operation")));
    }

    // convert native PG representaion to our local representation
    Int4RangeSet set1 = deserialize_ArrayType(input1, typcache);
    Int4RangeSet set2 = deserialize_ArrayType(input2, typcache);

    // callback function in this case is an arithmetic function with params: (Int4RangeSet a, Int4RangeSet b)
    int result;
    result = callback(set1, set2);

    return result;
}

//----------------------------------------------------------------------------
//-------------------------(De) Serialize functions---------------------------
//----------------------------------------------------------------------------

/*
Helper function -
    Take in RangeType of type typcache and return local definition of rangeset (I4RSet)
    Must eventually be freed by caller

    * Improvements: does properly handle empty case. Although our results wouldn't result in empty for the most part (i think)
    * handle NULL
*/
static Int4Range
deserialize_RangeType(RangeType *rng, TypeCacheEntry *typcache)
{
    Int4Range range;
    RangeBound lower, upper;
    bool isEmpty;

    range_deserialize(typcache, rng, &lower, &upper, &isEmpty);   

    // should we care to store empty ranges? same behavior as NULL
    if (isEmpty) {
        range.isNull = true;
        range.lower = 0;
        range.upper = 0;
        return range;
    }
    
    range.lower = DatumGetInt32(lower.val);
    range.upper = DatumGetInt32(upper.val);
    range.isNull = false;

    return range;
}

/*
Helper function -
    Take the Int4Range result of type typcache and return newly palloc RangeType
    Must eventually be freed by caller

    * Improvements: does properly handle empty case. Although our results wouldn't result in empty for the most part (i think)
    * handle NULL
*/
static RangeType*
serialize_RangeType(Int4Range range, TypeCacheEntry *typcache)
{
    // have no way to represent empty. Can represent NULL, but not empty
    // if (range.isempty???) does not exist
    // return result = make_range(typcache, &lower, &upper, true, NULL);

    // NULL range != empty range, how else to encode this?
    if (range.isNull) {
        return make_empty_range(typcache);
    }

    RangeType *result;
    RangeBound lower, upper;

    lower = make_range_bound(range.lower, true);
    upper = make_range_bound(range.upper, false);

    result = make_range(
        typcache,
        &lower,
        &upper,
        false,
        NULL
    );

    return result;
}

/*
Helper function -
    Take in ArrayType of type typcache and return local definition of rangeset (I4RSet)
    Must be freed by caller

    * confusion on how to handle many nulls in arry: [(1,3), NULL, NULL, NULL] => I4Rset {(1,3), NULL or more NULLS?}
     i think this is a slight enhancement tho bc it reduces extra calculations later on. redundant nulls dont mean anything new

    * Improvements: does not properly handle empty case. Although our results wouldn't result in empty for the most part (i think)
*/
static Int4RangeSet
deserialize_ArrayType(ArrayType *arr, TypeCacheEntry *typcache)
{
    Int4RangeSet set;

    // get type information. General this should be RangeType[] where RangeType varies
    Oid rangeTypeOID;
    int16 typlen;
    bool typbyval;
    char typalign;
    rangeTypeOID = typcache->type_id;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);
    
    // deconstruct array
    Datum *elems;
    bool *nulls;
    int count;
    deconstruct_array(arr, rangeTypeOID, typlen, typbyval, typalign, &elems, &nulls, &count);

    // initialize our representation of I4R
    set.containsNull = false;
    set.ranges = palloc(sizeof(Int4Range) * count);

    // go through every RangeType in array, and deserialize it
    int currIdx;
    int i;
    currIdx = 0;
    for (i = 0; i < count; i++) {
        
        // the RangeType at this index is NULL; trigger containsNull and set index isNull bool to true
        // only insert a NULL entry ONCE into the currIdx
        if (nulls[i] && !set.containsNull) {
            set.ranges[currIdx].lower = 0;
            set.ranges[currIdx].upper = 0; // maybe remove?
            set.ranges[currIdx].isNull = true;
            set.containsNull = true;
            currIdx++;
        }
        // otherwise Extract RangeType elements... only if not NULL. ignore nulls after first NULL found
        else if (!nulls[i]){
            RangeType *curr;
            RangeBound l1, u1;
            bool isEmpty;
            
            curr = DatumGetRangeTypeP(elems[i]);
            range_deserialize(typcache, curr, &l1, &u1, &isEmpty);

            // if the RangeType is not empty, append it 
            if (!isEmpty) {
                set.ranges[currIdx].lower = DatumGetInt32(l1.val);
                set.ranges[currIdx].upper = DatumGetInt32(u1.val);
                set.ranges[currIdx].isNull = false;
                currIdx++;
            }
        }
        // otherwise ignore
    }
    set.count = currIdx;

    pfree(elems);
    pfree(nulls);

    return set;
}

/*
Helper function -
    Take in ArrayType of type typcache and return local definition of rangeset (I4RSet)

    // Improvements: does not properly handle empty case. Although our results wouldn't result in empty for the most part (i think)
*/
static ArrayType* 
serialize_ArrayType(Int4RangeSet set, TypeCacheEntry *typcache)
{
    ArrayType *result;

    // must handle datums and nulls independently. get errors when calling range functions (make_range, range_deserialize) on nulls
    Datum *datums;
    bool  *nulls;
    
    datums = palloc(sizeof(Datum) * set.count);
    nulls  = palloc(sizeof(bool) * set.count);

    for(int i=0; i < set.count; i++) {
        // trigger NULL index properly
        if (set.ranges[i].isNull) {
            nulls[i] = true;
            datums[i] = (Datum) 0;      // NullableDatum Flag = 0
            continue;
        }
        
        nulls[i] = false;
            
        RangeBound lowerRv, upperRv;     
        lowerRv = make_range_bound(set.ranges[i].lower, true);
        upperRv = make_range_bound(set.ranges[i].upper, false);
        
        RangeType *r = make_range(typcache, &lowerRv, &upperRv, false, NULL);
        datums[i] = RangeTypePGetDatum(r);
    }

    int ndim;
    int dims[1];
    int lbs[1];
    
    ndim = 1;
    dims[0] = set.count;
    lbs[0] = 1;
    
    result = construct_md_array(
            datums, 
            nulls, 
            ndim, 
            dims, 
            lbs, 
            typcache->type_id, 
            typcache->typlen, 
            typcache->typbyval, 
            typcache->typalign
    );

    pfree(datums);
    pfree(nulls);

    return result;
}

/*
Helper function -
    Take in RangeType of type typcache and return local definition of rangeset (I4R)

    * Inclusive lower, exclusive upper
*/
static RangeBound make_range_bound(int32 val, bool is_lower)
{
    RangeBound rvBound;
    rvBound.val = Int32GetDatum(val);
    rvBound.inclusive = is_lower ? true : false;
    rvBound.infinite = false;
    rvBound.lower = is_lower;
    return rvBound;
}

/*
Helper function -
    Take in RangeType of type typcache and return local definition of rangeset (I4R)

    * Inclusive lower, exclusive upper
*/
static RangeBound make_closed_range_bound(int32 val, bool is_lower)
{
    RangeBound rvBound;
    rvBound.val = Int32GetDatum(val);
    rvBound.inclusive = true;
    rvBound.infinite = false;
    rvBound.lower = is_lower;
    return rvBound;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// TESTING
///////////////////////////////////////////////////////////////////////////////////////////////

ArrayType*
normalizeRange(ArrayType *input1) {
    Oid elemTypeOID;
    TypeCacheEntry *typcache;

    elemTypeOID = TypenameGetTypid("int4range");
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
Parameters: 
    ArrayType of Int4Ranges (RangeType) 
    Range Type for multiplicity [inclusive bounds]
*/
Datum
test_c_range_set_sum(PG_FUNCTION_ARGS)
{
    int resizeTrigger;
    resizeTrigger = 4;
    
    CHECK_BINARY_PGARG_NULL_ARGS();

    ArrayType *a1;
    ArrayType *a2;
    RangeBound *mult;
    
    a1 = PG_GETARG_ARRAYTYPE_P(0);
    a2 = PG_GETARG_ARRAYTYPE_P(1);
    mult = PG_GETARG_RANGE_P(2);

    ArrayType *sumOutput = arithmetic_set_helper(a1, a2, range_set_add);

    int nelems = ArrayGetNItems(ARR_NDIM(sumOutput), ARR_DIMS(sumOutput));

    ArrayType *rv;
    // call function to resize the array. create new arr and free old one
    if (nelems >= resizeTrigger) {
        // function to create new array
        rv = normalizeRange(sumOutput);
        pfree(sumOutput);
    }
    else {
        rv = sumOutput;
    }

    PG_RETURN_ARRAYTYPE_P(rv);
}

// agg over empty == 00
/* arbitrary trigger size. doesnt use for now 
    first parameter is the state
    second parameter is the current colA (I4RSet)
    third parameter is the multiplicity
 */
Datum
interval_agg_transfunc(PG_FUNCTION_ARGS) {
    IntervalAggState *state;
    ArrayType *new_rangeSet;
    RangeType *new_mult;
    RangeBound multLB, multUB;

    int triggerSize;
    triggerSize = 3;

    // inital state is {NULL, false}. Have to initalize on first run, otherwise get exsiting accumulated state
    if (PG_ARGISNULL(0)) {
        state->accumulated.count = 0;
        state->accumulated.ranges = NULL;
        state->accumulated.containsNull = false;

        state->mult.lower = 0;
        state->mult.upper = 0;
        state->mult.isNull = false;

        state->has_null = false;
        state->triggerSize = triggerSize;
    }
    else {
        state = (IntervalAggState*) PG_GETARG_POINTER(0);
    }

    // NULL input doesnt change state
    if(PG_ARGISNULL(1) || PG_ARGISNULL(2)) {
        PG_RETURN_POINTER(state);
    }
    
    new_rangeSet = PG_GETARG_ARRAYTYPE_P(1);
    new_mult = PG_GETARG_RANGE_P(2);

    // assign typcache based on RangeType input
    Oid rangeTypeOID, multTypeOID;
    TypeCacheEntry *rangeTypcache, *multTypcache;
    
    multTypeOID = RangeTypeGetOid(new_mult);
    rangeTypeOID = ARR_ELEMTYPE(new_rangeSet);
    multTypcache = lookup_type_cache(multTypeOID, TYPECACHE_RANGE_INFO);
    rangeTypcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

    Int4Range input_mult;
    Int4RangeSet input_set;
    RangeBound lb, ub;
    bool emptyMult;

    range_deserialize(multTypcache, new_mult, &lb, &ub, &emptyMult);
    input_mult.lower = lb.val;
    input_mult.upper = ub.val + 1;  // account for internal exclusive UB
    input_mult.isNull = false;

    input_set = deserialize_ArrayType(new_rangeSet, rangeTypcache);
    
    // check that either the left (range set) or right (mult) side can produce NULL. 
    // NULL remains possible only if previosuly possible and curently still possible
    state->has_null = state->has_null & (input_mult.lower == 0 || input_set.containsNull);

    // now we multiply the input_set by the multiplicity
    Int4RangeSet multipliedSet;
    multipliedSet = interval_agg_combine_set_mult(input_set, input_mult); // double check contains null and null handling
    pfree(input_set.ranges);

    // now add the accumulated rset in state with multipliedSet
    if (state->accumulated.count == 0) {
        state->accumulated = multipliedSet;
    }
    else{
        Int4RangeSet summed;

        summed = range_set_add(state->accumulated, multipliedSet);
        pfree(state->accumulated.ranges);
        pfree(multipliedSet.ranges);
        state->accumulated = summed;

        // FIXME
        // reduce size, not normalize if triggered
        if (state->accumulated.count >= state->triggerSize) {
            Int4RangeSet reduced;
            
            reduced = normalize(state->accumulated);
            pfree(state->accumulated.ranges);
            state->accumulated = reduced;
        }
    }
    PG_RETURN_POINTER(state);
}


// final func return accumulated 
ArrayType
interval_agg_transfunc() {

}
