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

PG_FUNCTION_INFO_V1(c_lift);
PG_FUNCTION_INFO_V1(c_sort);
PG_FUNCTION_INFO_V1(c_normalize);
PG_FUNCTION_INFO_V1(c_reduceSize);

PG_FUNCTION_INFO_V1(test_c_range_set_add);

// check for NULLS parameters. Different from empty range check
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

RangeType* arithmetic_helper(RangeType *r1, RangeType *r2, Int4Range (*callback)(Int4Range, Int4Range), char op);
ArrayType* arithmetic_set_helper( ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet) );
ArrayType* arithmetic_set_helperOp(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet), char op );
int comparison_helper(ArrayType *a1, ArrayType *a2, int (*callback)(Int4RangeSet, Int4RangeSet) );
ArrayType* general_helper( ArrayType *input, Int4RangeSet (*callback)() );


// deserialize_array_to_rangeset(A)


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
    
    output = arithmetic_helper(r1, r2, range_add, '+');

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

    output = arithmetic_helper(r1, r2, range_subtract, '-');

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

    output = arithmetic_helper(r1, r2, range_multiply, '*');

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

    output = arithmetic_helper(r1, r2, range_divide, '/');

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

    rv = comparison_helper(a1, a2, range_less_than);

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
    
    rv = comparison_helper(a1, a2, range_greater_than);
    
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

    rv = comparison_helper(a1, a2, range_less_than_equal);

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

    rv = comparison_helper(a1, a2, range_greater_than_equal);
    
    if (rv == -1){
        PG_RETURN_NULL();
    }

    PG_RETURN_BOOL((bool)rv);
}


/////////////////////
 // Helper Functions
/////////////////////

/* lift expects 1 parameter x for example and returns a valid int4range [x, x+1) */
Datum
c_lift_scalar(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    int unlifted = PG_GETARG_INT32(0);

    Int4Range rv = lift_scalar(unlifted);

    RangeBound lowerRv, upperRv;
    lowerRv.val = Int32GetDatum(rv.lower);
    lowerRv.inclusive = true;
    lowerRv.infinite = false;
    lowerRv.lower = true;

    upperRv.val = Int32GetDatum(rv.upper);
    upperRv.inclusive = false;
    upperRv.infinite = false;
    upperRv.lower = false;

    Oid rangeTypeOID = TypenameGetTypid("int4range");
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    
    RangeType *result = make_range(
        typcache, 
        &lowerRv, 
        &upperRv, 
        false,
        NULL
    );

    PG_RETURN_RANGE_P(result);
}

/* lift expects 1 parameter x for example and returns a valid int4range [x, x+1) */
// Datum
// c_lift_range(PG_FUNCTION_ARGS)
// {
//     // // check for NULLS. Diff from empty check
//     // if (PG_ARGISNULL(0)){
//     //     PG_RETURN_NULL();
//     // }

//     // // parse out single RangeType, convert to our I4R, call funciton and convert results back
//     // RangeType *r = PG_GETARG_RANGE_P(0);
//     // RangeBound l1, u1;
//     // bool isEmpty1;
//     // // require that typecache has range info
//     // Oid rangeTypeOID = TypenameGetTypid("int4range");
//     // TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
//     // range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);

//     // Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};

//     // Int4Range rv = lift_range(a);

//     // RangeBound lowerRv, upperRv;
//     // lowerRv.val = Int32GetDatum(rv.lower);
//     // lowerRv.inclusive = true;
//     // lowerRv.infinite = false;
//     // lowerRv.lower = true;

//     // upperRv.val = Int32GetDatum(rv.upper);
//     // upperRv.inclusive = false;
//     // upperRv.infinite = false;
//     // upperRv.lower = false;

//     // Oid rangeTypeOID = TypenameGetTypid("int4range");
//     // TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    
//     // RangeType *result = make_range(
//     //     typcache, 
//     //     &lowerRv, 
//     //     &upperRv, 
//     //     false, 
//     //     NULL
//     // );

//     // PG_RETURN_RANGE_P(result);


//     CHECK_BINARY_PGARG_NULL_OR();

//     RangeType *r = PG_GETARG_RANGE_P(0);

//     RangeType* rv = lift_helper(a1, lift_range);
    
//     PGreturn((bool)rv);
// }

Datum
c_reduceSize(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)){
        PG_RETURN_NULL();
    }

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    int32 numRangesKeep = PG_GETARG_INT32(1);

    Datum *elems1;
    bool *nulls1;
    int n1;

    // hardcoded for int4range. will need to replace for other range implementations
    // Oid rangeTypeOID = INT4RANGEOID;
    Oid rangeTypeOID = TypenameGetTypid("int4range");

    int16 typlen;
    bool typbyval;
    char typalign;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);

    deconstruct_array(a1, rangeTypeOID, typlen, typbyval, typalign, &elems1, &nulls1, &n1);
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0){
        PG_RETURN_NULL();
    }

    Int4RangeSet set1;
    set1.count = n1;
    set1.ranges = palloc(sizeof(Int4Range) * n1);

    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    // ereport(INFO, errmsg("(%d)", typcache->type_id));

    // add all values in param1 a1
    for(int i=0; i<n1; i++){
        RangeType *curr = DatumGetRangeTypeP(elems1[i]);
        RangeBound l1, u1;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l1, &u1, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set1.ranges[i].lower = DatumGetInt32(l1.val);
            set1.ranges[i].upper = DatumGetInt32(u1.val);
        } else {
            set1.ranges[i].lower = 0;
            set1.ranges[i].upper = 0;
        }
        // ereport(INFO, errmsg("(%d, %d)", set1.ranges[i].lower, set1.ranges[i].upper));
    }

    Int4RangeSet rv = reduceSize(set1, numRangesKeep);
    rv.count = numRangesKeep;

    Datum *results_out = palloc(sizeof(Datum) * rv.count);
    for(int i=0; i<rv.count; i++){
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
        results_out[i] = RangeTypePGetDatum(r);
    }

    // Convert array of Datums into an ArrayType
    ArrayType *resultsArrOut = construct_array(results_out, rv.count, rangeTypeOID, typlen, typbyval, typalign);

    // // implemented C functions use c memory allocation. Make sure to free!
    // if(rv.ranges){
    //     free(rv.ranges);
    // }

    PG_RETURN_ARRAYTYPE_P(resultsArrOut);
}

Datum
c_sort(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    ArrayType *input = PG_GETARG_ARRAYTYPE_P(0);
    Datum *elems;
    bool *nulls;
    int count;

    // deconstruct array
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    int16 typlen;
    bool typbyval;
    char typalign;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);
    deconstruct_array(input, rangeTypeOID, typlen, typbyval, typalign, &elems, &nulls, &count);
    
    ArrayType *output = general_helper(input, sort);

    PG_RETURN_ARRAYTYPE_P(output);
}

Datum
c_normalize(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    ArrayType *input = PG_GETARG_ARRAYTYPE_P(0);
    Datum *elems;
    bool *nulls;
    int count;

    // deconstruct array
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    int16 typlen;
    bool typbyval;
    char typalign;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);
    deconstruct_array(input, rangeTypeOID, typlen, typbyval, typalign, &elems, &nulls, &count);
    
    ArrayType *output = general_helper(input, normalize);

    PG_RETURN_ARRAYTYPE_P(output);
}


/*Takes in 2 parameters: Array: Int4RangeSet, and the function ptr callback: Int4RangeSet function() */
ArrayType*
general_helper(ArrayType *input, Int4RangeSet (*callback)() )
{
    Datum *elems;
    bool *nulls;
    int count;

    // deconstruct array
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    int16 typlen;
    bool typbyval;
    char typalign;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);
    deconstruct_array(input, rangeTypeOID, typlen, typbyval, typalign, &elems, &nulls, &count);

    // empty range
    if (count == 0){
        pfree(elems);
        pfree(nulls);
        return NULL;
    }

    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1;
    set1.count = count;
    set1.ranges = palloc(sizeof(Int4Range) * count);

    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

    // add all values in param1 a1
    for(int i=0; i<count; i++){
        RangeType *curr = DatumGetRangeTypeP(elems[i]);
        RangeBound l1, u1;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l1, &u1, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set1.ranges[i].lower = DatumGetInt32(l1.val);
            set1.ranges[i].upper = DatumGetInt32(u1.val);
        } else {
            set1.ranges[i].lower = 0;
            set1.ranges[i].upper = 0;
        }
        // ereport(INFO, errmsg("(%d, %d)", set1.ranges[i].lower, set1.ranges[i].upper));
    }

    Int4RangeSet rv = callback(set1);

    Datum *results_out = palloc(sizeof(Datum) * rv.count);
    for(int i=0; i<rv.count; i++){
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
        results_out[i] = RangeTypePGetDatum(r);
    }

    // Convert array of Datums into an ArrayType
    ArrayType *resultsArrOut = construct_array(results_out, rv.count, rangeTypeOID, typlen, typbyval, typalign);

    return resultsArrOut;
}

/*
Takes in 3 parameters: 
  s1 Array: Int4RangeSet, 
  s2 Array: Int4RangeSet, 
  function ptr callback: Int4RangeSet function()   
returns ArrayType result
*/
ArrayType*
arithmetic_set_helper(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet) )
{   
    // persist across diff func calls. Assumes only workign with I4R Oid
    // NOTE this is static for I4R. need a modular replacement for other range types
    static Oid rangeTypeOID = InvalidOid;
    static TypeCacheEntry *typcache = NULL;
    if (rangeTypeOID == InvalidOid) {
        rangeTypeOID = TypenameGetTypid("int4range");
        typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    }

    Datum *elems1, *elems2;
    bool *nulls1, *nulls2;
    int n1, n2;
    deconstruct_array(input1, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems1, &nulls1, &n1);
    deconstruct_array(input2, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems2, &nulls2, &n2);

    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // need to handle empty cases in array of ranges. either normalize before calculating or alternative..?

    // add all values in param1 a1
    for(int i=0; i<n1; i++){
        RangeType *curr = DatumGetRangeTypeP(elems1[i]);
        RangeBound l1, u1;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l1, &u1, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set1.ranges[i].lower = DatumGetInt32(l1.val);
            set1.ranges[i].upper = DatumGetInt32(u1.val);
        } else {
            set1.ranges[i].lower = 0;
            set1.ranges[i].upper = 0;
        }
    }

    for(int i=0; i<n2; i++){
        RangeType *curr = DatumGetRangeTypeP(elems2[i]);
        RangeBound l2, u2;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l2, &u2, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set2.ranges[i].lower = DatumGetInt32(l2.val);
            set2.ranges[i].upper = DatumGetInt32(u2.val);
        } else {
            set2.ranges[i].lower = 0;
            set2.ranges[i].upper = 0;
        }
    }
    
    // callback function in this case is an arithmetic function with params: (Int4RangeSet a, Int4RangeSet b)
    Int4RangeSet rv = callback(set1, set2);
    
    // convert reult of self defined type Int4RangeSet into a Datum array of RangeTypes
    Datum *results_out = palloc(sizeof(Datum) * rv.count);
    for(int i=0; i<rv.count; i++){
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
        results_out[i] = RangeTypePGetDatum(r);
    }

    // Convert array of Datums into an ArrayType
    ArrayType *resultsArrOut = construct_array(results_out, rv.count, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign);

    return resultsArrOut;
}

/*
Takes in 3 parameters: 
  s1 Array: Int4RangeSet, 
  s2 Array: Int4RangeSet, 
  function ptr callback: Int4RangeSet function()   
returns ArrayType result
*/
ArrayType*
arithmetic_set_helperOp(ArrayType *input1, ArrayType *input2, Int4RangeSet (*callback)(Int4RangeSet, Int4RangeSet), char op)
{   
    // persist across diff func calls. Assumes only workign with I4R Oid
    // NOTE this is static for I4R. need a modular replacement for other range types
    static Oid rangeTypeOID = InvalidOid;
    static TypeCacheEntry *typcache = NULL;
    if (rangeTypeOID == InvalidOid) {
        rangeTypeOID = TypenameGetTypid("int4range");
        typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    }

    Datum *elems1, *elems2;
    bool *nulls1, *nulls2;
    int n1, n2;
    deconstruct_array(input1, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems1, &nulls1, &n1);
    deconstruct_array(input2, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems2, &nulls2, &n2);

    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.containsNull = false;
    set2.containsNull = false;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // need to handle empty cases in array of ranges. either normalize before calculating or alternative..?

    // add all values in param1 input1
    for(int i=0; i<n1; i++){
        // the RangeType at this index is NULL
        if (nulls1[i]) {
            // maybe remove?
            set1.ranges[i].lower = 0;
            set1.ranges[i].upper = 0;

            set1.ranges[i].isNull = true;
            set1.containsNull = true;
        }
        // otherwise Extract RangeType elements
        else {
            RangeType *curr;
            RangeBound l1, u1;
            bool isEmpty1;
            
            curr = DatumGetRangeTypeP(elems1[i]);
            range_deserialize(typcache, curr, &l1, &u1, &isEmpty1);

            // need to handle is the RangeType is empty. 
            // if (!isEmpty) {}
            set1.ranges[i].lower = DatumGetInt32(l1.val);
            set1.ranges[i].upper = DatumGetInt32(u1.val);
            set1.ranges[i].isNull = false;
        }
    }

    // add all values in param2 input2
    for(int i=0; i<n2; i++){    
        // the RangeType at this index is NULL
        if (nulls2[i]) {
            // maybe remove?
            set2.ranges[i].lower = 0;
            set2.ranges[i].upper = 0;

            set2.ranges[i].isNull = true;
            set2.containsNull = true;
        }
        // otherwise Extract RangeType elements
        else {
            RangeType *curr;
            RangeBound l2, u2;
            bool isEmpty2;
            
            curr = DatumGetRangeTypeP(elems2[i]);
            range_deserialize(typcache, curr, &l2, &u2, &isEmpty2);

            // need to handle is the RangeType is empty. 
            // if (!isEmpty) {}
            set2.ranges[i].lower = DatumGetInt32(l2.val);
            set2.ranges[i].upper = DatumGetInt32(u2.val);
            set2.ranges[i].isNull = false;
        }
    }
    
    // callback function in this case is an arithmetic function with params: (Int4RangeSet a, Int4RangeSet b)
    Int4RangeSet rv;
    rv = callback(set1, set2);
    
    Datum *datums;
    bool  *nulls;
    datums = palloc(sizeof(Datum) * rv.count);
    nulls  = palloc(sizeof(bool) * rv.count);

    for(int i=0; i<rv.count; i++){
        // trigger NULL index properly
        if (rv.ranges[i].isNull) {
            nulls[i] = true;
            datums[i] = (Datum) 0;
            continue;
        }
        
        nulls[i] = false;

        RangeBound lowerRv, upperRv;        
        // lowerRv.val = Int32GetDatum(rv.ranges[i].lower);
        lowerRv.val = Int32GetDatum(rv.ranges[i].lower);    
        lowerRv.inclusive = true;
        lowerRv.infinite = false;
        lowerRv.lower = true;
        
        // upperRv.val = Int32GetDatum(rv.ranges[i].upper);
        upperRv.val = Int32GetDatum(rv.ranges[i].upper);
        upperRv.inclusive = false;
        upperRv.infinite = false;
        upperRv.lower = false;

        RangeType *r = make_range(typcache, &lowerRv, &upperRv, false, NULL);
        datums[i] = RangeTypePGetDatum(r);
    }

    int ndim;
    int dims[1];
    int lbs[1];
    
    ndim = 1;
    dims[0] = rv.count;
    lbs[0] = 1;
    
    ArrayType *resultsArrOut = construct_md_array(datums, nulls, ndim, dims, lbs, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign);

    pfree(elems1);
    pfree(nulls1);
    pfree(elems2);
    pfree(nulls2);
    pfree(set1.ranges);
    pfree(set2.ranges);

    // Convert array of Datums into an ArrayType
    // ArrayType *resultsArrOut = construct_array(results_out, rv.count, rangeTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign);

    return resultsArrOut;
}

/*
Takes in 4 parameters: 
  s1 Array: Int4RangeSet, 
  s2 Array: Int4RangeSet, 
  function ptr callback: Int4RangeSet function()   
  op: char (+, -, *, /)         ** added to prevent error on empty cases
returns ArrayType result
*/
RangeType*
arithmetic_helper(RangeType *r1, RangeType *r2, Int4Range (*callback)(Int4Range, Int4Range), char op)
{   
    // persist across diff func calls. Assumes only workign with I4R Oid
    // NOTE this is static for I4R. need a modular replacement for other range types
    static Oid rangeTypeOID = InvalidOid;
    static TypeCacheEntry *typcache = NULL;
    
    
    // can maybe set oid if r1->rangetypid == r1->rangetypid then rangeTypeOID = r1->rangetypid;
    if (rangeTypeOID == InvalidOid) {
        rangeTypeOID = TypenameGetTypid("int4range");
        typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    }
    
    RangeBound l1, u1, l2, u2;
    bool isEmpty1, isEmpty2;
    range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);
    range_deserialize(typcache, r2, &l2, &u2, &isEmpty2);

    // Handle empty/ NULL values. Acts diff on operations
    if (isEmpty1 && isEmpty2){
        return make_empty_range(typcache);
    }
    else if (op == '+'){
        if (isEmpty1) return r2;
        if (isEmpty2) return r1;
    }
    else if (op == '-'){
        if (isEmpty1) return make_empty_range(typcache);
        if (isEmpty2) return r1;
    }
    else if (op == '*' || op == '/') {
        if (isEmpty1 || isEmpty2) return make_empty_range(typcache);
    }
    
    Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};
    Int4Range b = {DatumGetInt32(l2.val), DatumGetInt32(u2.val)};

    if(!validRange(a) || !validRange(b)) {
        return make_empty_range(typcache);;
    }

    // implemented C function
    Int4Range rv = callback(a, b);

    // create rv and set rv's attributes
    RangeBound lowerRv, upperRv;
    lowerRv.val = Int32GetDatum(rv.lower);
    lowerRv.inclusive = true;
    lowerRv.infinite = false;
    lowerRv.lower = true;
    upperRv.val = Int32GetDatum(rv.upper);
    upperRv.inclusive = false;
    upperRv.infinite = false;
    upperRv.lower = false;
    
    RangeType *result = make_range(
        typcache, 
        &lowerRv, 
        &upperRv, 
        false,
        NULL
    );
    
    return result;
}

int 
comparison_helper(ArrayType *a1, ArrayType *a2, int (*callback)(Int4RangeSet, Int4RangeSet) )
{
    Datum *elems1, *elems2;
    bool *nulls1, *nulls2;
    int n1, n2;

    // hardcoded for int4range. will need to replace for other range implementations
    // Oid rangeTypeOID = INT4RANGEOID;
    Oid rangeTypeOID = TypenameGetTypid("int4range");

    int16 typlen;
    bool typbyval;
    char typalign;
    get_typlenbyvalalign(rangeTypeOID, &typlen, &typbyval, &typalign);

    deconstruct_array(a1, rangeTypeOID, typlen, typbyval, typalign, &elems1, &nulls1, &n1);
    deconstruct_array(a2, rangeTypeOID, typlen, typbyval, typalign, &elems2, &nulls2, &n2);
    
    // // NULL on both empty, return non empty otherwise. 
    // if (n1 == 0 && n2 == 0){
    //     PG_RETURN_NULL();
    // }
    // else if (n1 == 0){
    //     PG_RETURN_ARRAYTYPE_P(a2);
    // }
    // else if (n2 == 0){
    //     PG_RETURN_ARRAYTYPE_P(a1);
    // }
    
    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    // ereport(INFO, errmsg("(%d)", typcache->type_id));

    // add all values in param1 a1
    for(int i=0; i<n1; i++){
        RangeType *curr = DatumGetRangeTypeP(elems1[i]);
        RangeBound l1, u1;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l1, &u1, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set1.ranges[i].lower = DatumGetInt32(l1.val);
            set1.ranges[i].upper = DatumGetInt32(u1.val);
        } else {
            set1.ranges[i].lower = 0;
            set1.ranges[i].upper = 0;
        }
        // ereport(INFO, errmsg("(%d, %d)", set1.ranges[i].lower, set1.ranges[i].upper));
    }

    for(int i=0; i<n2; i++){
        RangeType *curr = DatumGetRangeTypeP(elems2[i]);
        RangeBound l2, u2;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l2, &u2, &isEmpty);
        
        // if range 1 not empty then set curr index low and high members. default 0, 0 has no effect on add
        if (!isEmpty) {
            set2.ranges[i].lower = DatumGetInt32(l2.val);
            set2.ranges[i].upper = DatumGetInt32(u2.val);
        } else {
            set2.ranges[i].lower = 0;
            set2.ranges[i].upper = 0;
        }
        // ereport(INFO, errmsg("(%d, %d)", set1.ranges[i].lower, set1.ranges[i].upper));
    }

    // convert from defined Int4RangeSet into a Datum array of RangeTypes
    int rv = range_less_than(set1, set2);
    
    // otherwise return the datum representation of boolean result
    return rv;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// TESTING
///////////////////////////////////////////////////////////////////////////////////////////////

ArrayType*
normalizeRange(ArrayType *input1) {
    Oid elemTypeOID;
    TypeCacheEntry *typcache;
    // Get the element type (should be range type)
    elemTypeOID = ARR_ELEMTYPE(input1);
    typcache = lookup_type_cache(elemTypeOID, TYPECACHE_RANGE_INFO);
    
    // deconstruct array, create our representation of I4R, call function and get 'normalized' result
    Datum *elems1;
    bool *nulls1;
    int n1;

    deconstruct_array(input1, elemTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign, &elems1, &nulls1, &n1);

    // Our representation of I4R. Should be freed after normalizing/reducing to potentially smaller range
    Int4RangeSet set1;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    
    int currIdx = 0;
    for(int i=0; i<n1; i++){
        // only append in valid ranges
        RangeType *curr = DatumGetRangeTypeP(elems1[i]);
        RangeBound l1, u1;
        bool isEmpty;
        
        range_deserialize(typcache, curr, &l1, &u1, &isEmpty);
        
        if (!isEmpty) {
            set1.ranges[currIdx].lower = DatumGetInt32(l1.val);
            set1.ranges[currIdx].upper = DatumGetInt32(u1.val);
            currIdx++;
        }
    }
    set1.count = currIdx;
    
    Int4Range *temp;
    // what to do here?
    if (set1.count == 0) {
        pfree(set1.ranges);
        return construct_empty_array(elemTypeOID);
    }
    // change size of working set after removing NULL's
    else {
        temp = repalloc(set1.ranges, sizeof(Int4Range) * set1.count);
        if (temp != NULL) set1.ranges = temp;
    }

    // Remove all possible overlap.
    Int4RangeSet outSet = normalize(set1);

    // convert self defined type into valid Postgres Type
    Datum *results_out = palloc(sizeof(Datum) * outSet.count);
    for(int i=0; i<outSet.count; i++){
        RangeBound lowerRv, upperRv;
        lowerRv.val = Int32GetDatum(outSet.ranges[i].lower);
        lowerRv.inclusive = true;
        lowerRv.infinite = false;
        lowerRv.lower = true;

        upperRv.val = Int32GetDatum(outSet.ranges[i].upper);
        upperRv.inclusive = false;
        upperRv.infinite = false;
        upperRv.lower = false;

        // wrong typcache?? 
        RangeType *r = make_range(typcache, &lowerRv, &upperRv, false, NULL);
        results_out[i] = RangeTypePGetDatum(r);
    }

    // Convert array of Datums into an ArrayType
    ArrayType *resultsArrOut = construct_array(results_out, outSet.count, elemTypeOID, typcache->typlen, typcache->typbyval, typcache->typalign);
    
    pfree(elems1);
    pfree(nulls1);
    pfree(set1.ranges);
    pfree(results_out);

    return resultsArrOut;
}





/*
Parameters: 
    ArrayType of Int4Ranges (RangeType) 
    Range Type for multiplicity [inclusive bounds]
*/
Datum
test_c_range_set_add(PG_FUNCTION_ARGS)
{
    int resizeTrigger = 4;
    
    CHECK_BINARY_PGARG_NULL_ARGS();

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);
    
    ArrayType *output = arithmetic_set_helper(a1, a2, range_set_add);

    int nelems = ArrayGetNItems(ARR_NDIM(output), ARR_DIMS(output));

    ArrayType *rv;
    // call function to resize the array. create new arr and free old one
    if (nelems >= resizeTrigger) {
        // function to create new array
        rv = normalizeRange(output);
        pfree(output);
    }
    else {
        rv = output;
    }

    PG_RETURN_ARRAYTYPE_P(rv);
}


// c_sum_func(c_range_mult(colA, multiplicity))
