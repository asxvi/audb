-- `complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION i4r_arithmetic" to load this file. \quit

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

-- c_range_subtract takes 2 int4range types and returns the product
CREATE FUNCTION c_range_multiply(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_multiply'
LANGUAGE c;

-- c_range_subtract takes 2 int4range types and returns the divded result
CREATE FUNCTION c_range_divide(a int4range, b int4range ) 
RETURNS int4range
AS 'MODULE_PATHNAME', 'c_range_divide'
LANGUAGE c;


----------------------------------------------------------------------------
-------------------------------Set functions--------------------------------
----------------------------------------------------------------------------
-- c_range_subtract takes 2 int4range types and returns the divded result
CREATE FUNCTION c_range_set_add(a int4range[], b int4range[])
RETURNS int4range[]
AS 'MODULE_PATHNAME', 'c_range_set_add'
LANGUAGE c;