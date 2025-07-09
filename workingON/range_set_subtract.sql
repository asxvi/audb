CREATE OR REPLACE FUNCTION range_subtract(set1 int4range, set2 int4range) 
RETURNS int4range as $$
BEGIN
    
    RETURN
