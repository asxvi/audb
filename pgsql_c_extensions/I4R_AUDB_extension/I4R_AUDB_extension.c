#include "arithmetic.h"         // c implemented I4R functions
#include "logicalOperators.h"

#include "postgres.h"
#include "fmgr.h"

#include "utils/rangetypes.h"   // non set functions pass RangeType params
#include "utils/array.h"        // set functions pass ArrayType params
#include "utils/typcache.h"
#include "utils/lsyscache.h"    // "Convenience routines for common queries in the system catalog cache."
#include "catalog/pg_type_d.h"  // pg_type oid macros
#include "catalog/namespace.h"
#include "utils/rangetypes.h"

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

PG_FUNCTION_INFO_V1(lift);
PG_FUNCTION_INFO_V1(sort);
PG_FUNCTION_INFO_V1(normalize);
PG_FUNCTION_INFO_V1(reduceSize);

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
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

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
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

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
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

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
    Oid rangeTypeOID = TypenameGetTypid("int4range");
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);

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


/*
    takes in 2 pg Arrays of RangeType parameters, and returns
    a single array of RangeType with added results
*/
Datum
c_range_set_add(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    ereport(INFO, errmsg("(%d)", typcache->type_id));

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
    Int4RangeSet rv = range_set_add(set1, set2);
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

    // implemented C functions use c memory allocation. Make sure to free!
    if(rv.ranges){
        free(rv.ranges);
    }

    PG_RETURN_ARRAYTYPE_P(resultsArrOut);
}

/*
    takes in 2 pg Arrays of RangeType parameters, and returns
    a single array of RangeType with subtracted results
*/
Datum
c_range_set_subtract(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    ereport(INFO, errmsg("(%d)", typcache->type_id));

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
    Int4RangeSet rv = range_set_subtract(set1, set2);
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

    // implemented C functions use c memory allocation. Make sure to free!
    if(rv.ranges){
        free(rv.ranges);
    }

    PG_RETURN_ARRAYTYPE_P(resultsArrOut);
}

/*
    takes in 2 pg Arrays of RangeType parameters, and returns
    a single array of RangeType with multiplied results
*/
Datum
c_range_set_multiply(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
    // create self defined Int4RangeSet type, and convert Datum into RangeType appending to set
    Int4RangeSet set1, set2;
    set1.count = n1;
    set2.count = n2;
    set1.ranges = palloc(sizeof(Int4Range) * n1);
    set2.ranges = palloc(sizeof(Int4Range) * n2);
    
    // require that typecache has range info
    TypeCacheEntry *typcache = lookup_type_cache(rangeTypeOID, TYPECACHE_RANGE_INFO);
    ereport(INFO, errmsg("(%d)", typcache->type_id));

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
    Int4RangeSet rv = range_set_multiply(set1, set2);
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

    // implemented C functions use c memory allocation. Make sure to free!
    if(rv.ranges){
        free(rv.ranges);
    }

    PG_RETURN_ARRAYTYPE_P(resultsArrOut);
}

/*
    takes in 2 pg Arrays of RangeType parameters, and returns
    a single array of RangeType with multiplied results
*/
Datum
c_range_set_divide(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_NULL();
    else if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    else if (PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
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
    Int4RangeSet rv = range_set_divide(set1, set2);

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

    // implemented C functions use c memory allocation. Make sure to free!
    if(rv.ranges){
        free(rv.ranges);
    }

    PG_RETURN_ARRAYTYPE_P(resultsArrOut);
}


/////////////////////
    // Comparison
/////////////////////

Datum
c_lt(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();
    // else if (PG_ARGISNULL(0))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    // else if (PG_ARGISNULL(1))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
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

    if (rv == -1){
        PG_RETURN_NULL();
    }
    
    // otherwise return the datum representation of boolean result

    // Datum rvDatum = BoolGetDatum((bool)rv);
    // PG_RETURN_DATUM(rv);
    PG_RETURN_BOOL((bool)rv);
}

Datum
c_lte(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();
    // else if (PG_ARGISNULL(0))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    // else if (PG_ARGISNULL(1))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
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
    int rv = range_less_than_equal(set1, set2);

    if (rv == -1){
        PG_RETURN_NULL();
    }
    
    // otherwise return the datum representation of boolean result

    // Datum rvDatum = BoolGetDatum((bool)rv);
    PG_RETURN_BOOL((bool)rv);
}

Datum
c_gt(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();
    // else if (PG_ARGISNULL(0))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    // else if (PG_ARGISNULL(1))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
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
    int rv = range_greater_than(set1, set2);

    if (rv == -1){
        PG_RETURN_NULL();
    }
    
    // otherwise return the datum representation of boolean result

    // Datum rvDatum = BoolGetDatum((bool)rv);
    // PG_RETURN_DATUM(rv);
    PG_RETURN_BOOL((bool)rv);
}

Datum
c_gte(PG_FUNCTION_ARGS)
{   
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();
    // else if (PG_ARGISNULL(0))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    // else if (PG_ARGISNULL(1))
    //     PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    ArrayType *a1 = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *a2 = PG_GETARG_ARRAYTYPE_P(1);

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
    
    // NULL on both empty, return non empty otherwise. 
    if (n1 == 0 && n2 == 0){
        PG_RETURN_NULL();
    }
    else if (n1 == 0){
        PG_RETURN_ARRAYTYPE_P(a2);
    }
    else if (n2 == 0){
        PG_RETURN_ARRAYTYPE_P(a1);
    }
    
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
    int rv = range_greater_than_equal(set1, set2);

    if (rv == -1){
        PG_RETURN_NULL();
    }
    
    // otherwise return the datum representation of boolean result

    // Datum rvDatum = BoolGetDatum((bool)rv);
    PG_RETURN_BOOL((bool)rv);
}

// lift expects 1 parameter x for example and returns a valid int4range [x, x+1)
Datum
lift(PG_FUNCTION_ARGS)
{
    // check for NULLS. Diff from empty check
    if (PG_ARGISNULL(0)){
        PG_RETURN_NULL();
    }

    PG_

}

Datum
sort(PG_FUNCTION_ARGS)
{


}

Datum
normalize(PG_FUNCTION_ARGS)
{
    

}

Datum
reduceSize(PG_FUNCTION_ARGS)
{
    

}