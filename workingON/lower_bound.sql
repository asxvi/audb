CREATE OR REPLACE FUNCTION lower_bound(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    RETURN 