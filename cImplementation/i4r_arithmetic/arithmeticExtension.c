#include "postgres.h"
#include "fmgr.h"
#include "utils/rangetypes.h"
#include "utils/typcache.h"
#include "catalog/pg_type_d.h" // pg_type oid macros
#include "arithmetic.h"
#include "utils/rangetypes.h"


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(c_range_add);
PG_FUNCTION_INFO_V1(c_range_subtract);
PG_FUNCTION_INFO_V1(c_range_multiply);
PG_FUNCTION_INFO_V1(c_range_divide);


/*
    takes in 2 pg RangeType parameters, and returns
    a single RangeType with added result
*/
Datum
c_range_add(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    RangeType *r1 = PG_GETARG_RANGE_P(0);
    RangeType *r2 = PG_GETARG_RANGE_P(1);

    RangeBound l1, u1, l2, u2;
    bool isEmpty1, isEmpty2;
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(INT4RANGEOID, TYPECACHE_RANGE_INFO);

    range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);
    range_deserialize(typcache, r2, &l2, &u2, &isEmpty2);

    // NULL on both empty, return non empty otherwise. 
    if (isEmpty1 && isEmpty2){
        PG_RETURN_NULL();
    }
    else if (isEmpty1){
        PG_RETURN_RANGE_P(r2);
    }
    else if (isEmpty2){
        PG_RETURN_RANGE_P(r1);
    }
    
    Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};
    Int4Range b = {DatumGetInt32(l2.val), DatumGetInt32(u2.val)};

    // implemented C function
    Int4Range rv = range_add(a, b);

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
    
    PG_RETURN_RANGE_P(result);
}

/*
    takes in 2 pg RangeType parameters, and returns
    a single RangeType with subtracted result
*/
Datum
c_range_subtract(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    RangeType *r1 = PG_GETARG_RANGE_P(0);
    RangeType *r2 = PG_GETARG_RANGE_P(1);

    RangeBound l1, u1, l2, u2;
    bool isEmpty1, isEmpty2;
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(INT4RANGEOID, TYPECACHE_RANGE_INFO);

    range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);
    range_deserialize(typcache, r2, &l2, &u2, &isEmpty2);

    // NULL on both empty, return non empty otherwise. 
    if (isEmpty1 && isEmpty2){
        PG_RETURN_NULL();
    }
    else if (isEmpty1){
        PG_RETURN_RANGE_P(r2);
    }
    else if (isEmpty2){
        PG_RETURN_RANGE_P(r1);
    }
    
    Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};
    Int4Range b = {DatumGetInt32(l2.val), DatumGetInt32(u2.val)};

    // implemented C function
    Int4Range rv = range_subtract(a, b);

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
    
    PG_RETURN_RANGE_P(result);
}

/*
    takes in 2 pg RangeType parameters, and returns
    a single RangeType with mutiplied result
*/
Datum
c_range_multiply(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    RangeType *r1 = PG_GETARG_RANGE_P(0);
    RangeType *r2 = PG_GETARG_RANGE_P(1);

    RangeBound l1, u1, l2, u2;
    bool isEmpty1, isEmpty2;
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(INT4RANGEOID, TYPECACHE_RANGE_INFO);

    range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);
    range_deserialize(typcache, r2, &l2, &u2, &isEmpty2);

    // NULL on both empty, return non empty otherwise. 
    if (isEmpty1 && isEmpty2){
        PG_RETURN_NULL();
    }
    else if (isEmpty1){
        PG_RETURN_RANGE_P(r2);
    }
    else if (isEmpty2){
        PG_RETURN_RANGE_P(r1);
    }
    
    Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};
    Int4Range b = {DatumGetInt32(l2.val), DatumGetInt32(u2.val)};

    // implemented C function
    Int4Range rv = range_multiply(a, b);

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
    
    PG_RETURN_RANGE_P(result);
}

/*
    takes in 2 pg RangeType parameters, and returns
    a single RangeType with divided result
*/
Datum
c_range_divide(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    RangeType *r1 = PG_GETARG_RANGE_P(0);
    RangeType *r2 = PG_GETARG_RANGE_P(1);

    RangeBound l1, u1, l2, u2;
    bool isEmpty1, isEmpty2;
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(INT4RANGEOID, TYPECACHE_RANGE_INFO);

    range_deserialize(typcache, r1, &l1, &u1, &isEmpty1);
    range_deserialize(typcache, r2, &l2, &u2, &isEmpty2);

    // NULL on both empty, return non empty otherwise. 
    if (isEmpty1 && isEmpty2){
        PG_RETURN_NULL();
    }
    else if (isEmpty1){
        PG_RETURN_RANGE_P(r2);
    }
    else if (isEmpty2){
        PG_RETURN_RANGE_P(r1);
    }
    
    Int4Range a = {DatumGetInt32(l1.val), DatumGetInt32(u1.val)};
    Int4Range b = {DatumGetInt32(l2.val), DatumGetInt32(u2.val)};

    // implemented C function
    Int4Range rv = range_divide(a, b);

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
    
    PG_RETURN_RANGE_P(result);
}
