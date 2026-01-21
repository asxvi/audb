-- `complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION i4r_audb_extension" to load this file. \quit


----------------------------------------------------------------------------
---------------------------------TESTING------------------------------------

-- test_c_range_set_sum takes 2 int4range types and returns the sum
CREATE FUNCTION test_c_range_set_sum(a int4range[], b int4range[]) 
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'test_c_range_set_sum'
LANGUAGE c;
-- LANGUAGE c STRICT VOLATILE;

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