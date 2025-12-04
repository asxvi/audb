-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION c_make_title" to load this file. \quit

-- convert_to_title will take a string and convert the initial characters of each word to a capital letter
CREATE FUNCTION c_convert_to_title(inp text) RETURNS text
AS 'MODULE_PATHNAME', 'c_convert_to_title'
LANGUAGE c STRICT VOLATILE;