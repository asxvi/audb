-- `complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION i4r_audb_extension" to load this file. \quit

----------------------------------------------------------------------------
---------------------------Arithemtic functions-----------------------------
----------------------------------------------------------------------------

-- c_range_add takes 2 int4range types and returns the sum
CREATE FUNCTION c_range_add(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_add'
LANGUAGE c;
-- LANGUAGE c STRICT VOLATILE;

-- c_range_subtract takes 2 int4range types and returns the difference
CREATE FUNCTION c_range_subtract(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_subtract'
LANGUAGE c;

-- c_range_multiply takes 2 int4range types and returns the product
CREATE FUNCTION c_range_multiply(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_multiply'
LANGUAGE c;

-- c_range_divide takes 2 int4range types and returns the divded result
CREATE FUNCTION c_range_divide(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_divide'
LANGUAGE c;

-- c_range_set_add takes 2 arrays of int4range types and returns an array with added results
CREATE FUNCTION c_range_set_add(a int4range[], b int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_range_set_add'
LANGUAGE c;

-- c_range_set_subtract takes 2 arrays of int4range types and returns an array with subtracted results
CREATE FUNCTION c_range_set_subtract(a int4range[], b int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_range_set_subtract'
LANGUAGE c;

-- c_range_set_multiply takes 2 arrays of int4range types and returns an array with subtracted results
CREATE FUNCTION c_range_set_multiply(a int4range[], b int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_range_set_multiply'
LANGUAGE c;

-- c_range_set_divide takes 2 arrays of int4range types and returns an array with subtracted results
CREATE FUNCTION c_range_set_divide(a int4range[], b int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_range_set_divide'
LANGUAGE c;


-- ----------------------------------------------------------------------------
-- -------------------------Logical Operator functions-------------------------
-- ----------------------------------------------------------------------------

-- c_lt takes 2 arrays of int4range types and returns bool result of logical expression
CREATE FUNCTION c_lt(a int4range[], b int4range[])
RETURNS boolean
AS 'MODULE_PATHNAME', 'c_lt'
LANGUAGE c;

-- c_lte takes 2 arrays of int4range types and returns bool result of logical expression
CREATE FUNCTION c_lte(a int4range[], b int4range[])
RETURNS boolean
AS 'MODULE_PATHNAME', 'c_lte'
LANGUAGE c;

-- c_gt takes 2 arrays of int4range types and returns bool result of logical expression
CREATE FUNCTION c_gt(a int4range[], b int4range[])
RETURNS boolean
AS 'MODULE_PATHNAME', 'c_gt'
LANGUAGE c;

-- c_gte takes 2 arrays of int4range types and returns bool result of logical expression
CREATE FUNCTION c_gte(a int4range[], b int4range[])
RETURNS boolean
AS 'MODULE_PATHNAME', 'c_gte'
LANGUAGE c;

-- ----------------------------------------------------------------------------
-- ------------------------------Helper functions------------------------------
-- ----------------------------------------------------------------------------

-- c_lift takes 1 int32 and returns its equivallent Int4Range 
CREATE FUNCTION c_lift_scalar(a int4)
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_lift_scalar'
LANGUAGE c;

-- -- c_lift takes 1 int4range and returns its equivallent Int4RangeSet: int4range[] 
-- CREATE FUNCTION c_lift_range(a int4range)
-- RETURNS int4range[]
-- AS 'MODULE_PATHNAME', 'c_lift_range'
-- LANGUAGE c;

-- c_reduceSize takes 1 array of int4range, and an integer and returns reduced size array of int4range
CREATE FUNCTION c_reduceSize(a int4range[], numRangesKeep integer)
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_reduceSize'
LANGUAGE c;

-- c_sort takes 1 array of int4range, and sorts input 
CREATE FUNCTION c_sort(a int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_sort'
LANGUAGE c;

-- c_normalize takes 1 array of int4range, and merges contained ranges
CREATE FUNCTION c_normalize(a int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_normalize'
LANGUAGE c;




----------------------------------------------------------------------------
-------------------------------Aggregates-----------------------------------


---------- SUM -----------

-- -- test_c_range_set_sum takes 2 int4range types and returns the sum
-- CREATE FUNCTION test_c_range_set_sum(a int4range[], b int4range[]) 
-- RETURNS int4range[]
-- AS 'MODULE_PATHNAME', 'test_c_range_set_sum'
-- LANGUAGE c;
-- -- LANGUAGE c STRICT VOLATILE;

CREATE FUNCTION agg_sum_interval_transfunc(internal, int4range[], int4range) 
RETURNS internal
AS 'MODULE_PATHNAME', 'agg_sum_interval_transfunc'
LANGUAGE c;
-- LANGUAGE c STRICT VOLATILE;

CREATE FUNCTION agg_sum_interval_finalfunc(internal) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'agg_sum_interval_finalfunc'
LANGUAGE c;
-- LANGUAGE c STRICT VOLATILE;

create aggregate sum (int4range[], int4range)
(
    stype = internal,       -- Type: IntervalAggState
    sfunc = agg_sum_interval_transfunc,
    finalfunc = agg_sum_interval_finalfunc
);

---------- MIN/ MAX -----------
-- not sure if this is more functions than need be

CREATE FUNCTION combine_range_mult_min(int4range, int4range) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'combine_range_mult_min'
LANGUAGE c;

CREATE FUNCTION combine_range_mult_max(int4range, int4range) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'combine_range_mult_max'
LANGUAGE c;

CREATE FUNCTION agg_min_transfunc(int4range, int4range) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'agg_min_transfunc'
LANGUAGE c;

CREATE FUNCTION agg_max_transfunc(int4range, int4range) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'agg_max_transfunc'
LANGUAGE c;

create aggregate min (int4range)
(
    stype = int4range,
    sfunc = agg_min_transfunc
);
create aggregate max (int4range)
(
    stype = int4range,
    sfunc = agg_max_transfunc
);

CREATE FUNCTION combine_set_mult_min(int4range[], int4range) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'combine_set_mult_min'
LANGUAGE c;

CREATE FUNCTION combine_set_mult_max(int4range[], int4range) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'combine_set_mult_max'
LANGUAGE c;

CREATE FUNCTION agg_set_min_transfunc(int4range[], int4range[]) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'agg_set_min_transfunc'
LANGUAGE c;

CREATE FUNCTION agg_set_max_transfunc(int4range[], int4range[]) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'agg_set_max_transfunc'
LANGUAGE c;

CREATE FUNCTION set_min_max_finalfunc(int4range[]) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'set_min_max_finalfunc'
LANGUAGE c;

create aggregate min (int4range[])
(
    stype = int4range[],
    sfunc = agg_set_min_transfunc,
    finalfunc = set_min_max_finalfunc
);
create aggregate max (int4range[])
(
    stype = int4range[],
    sfunc = agg_set_max_transfunc,
    finalfunc = set_min_max_finalfunc
);















-- init test data

DROP TABLE IF EXISTS r1;
DROP TABLE IF EXISTS s1;

CREATE TABLE IF NOT EXISTS r1(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range,
    colB int4range,
    mult int4range
);

INSERT INTO r1 (colA, colB, mult) VALUES
    (int4range(1,1000), int4range(200,400), int4range(0,2)),
    (int4range(9,11), int4range(4,9), int4range(1,2)),
    (int4range(10,13), int4range(1,12), int4range(1,7)),
    (int4range(100,130), int4range(12,1400), int4range(6,7)),
    (int4range(6,7), int4range(121,122), int4range(2,4)),
    (int4range(44,332), int4range(12,14), int4range(5,6)),
    ('empty'::int4range, int4range(23,34), int4range(5,6)),
    (int4range(24,34), 'empty'::int4range, int4range(5,6));


CREATE TABLE IF NOT EXISTS s1(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range[],
    colB int4range[],
    mult int4range
);

INSERT INTO s1 (colA, colB, mult) VALUES
    (array[int4range(1,5), int4range(9,12), int4range(18,29)], array[int4range(4,13), int4range(16,20)], int4range(1,1, '[]')),
    (array[int4range(4,13), int4range(16,20)], array[int4range(22,25), int4range(34,50)], int4range(1,1, '[]')),
    (array[int4range(1,5), int4range(9,12), int4range(18,29)], array[int4range(4,13), int4range(16,20)], int4range(0,1, '[]'));



