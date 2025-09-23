
-- does not split ranges how it should bc native function - doesn't support this. Plan to implement
-- ex
CREATE OR REPLACE FUNCTION range_set_not(set1 int4range[], set2 int4range[]) RETURNS int4range[] AS $$
DECLARE
    result int4range[];
BEGIN
    result := ARRAY[]::int4range[];
    FOR i IN 1..array_length(set1, 1) LOOP
        result := array_append(result, set1[i] - set2[i]);
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;


-- SELECT (range_set_not(
--         ARRAY[int4range(1,4), int4range(6,9), int4range(2,5)],
--         ARRAY[int4range(100,1220), int4range(3, 6)]
-- )) as jack;



-- SELECT (range_set_not(
--         ARRAY[int4range(1,10)],
--         ARRAY[int4range(4,6)]
-- )) as jack;



-- create or replace function range_sub(a int4range, b int4range) 
-- returns int4range[] as $$
-- DECLARE
--     rv int4range[];
-- BEGIN
--     IF isempty(a) or isempty(b) OR NOT a && b THEN
--         result := array(a);
--     ELSIF 

    
-- end
-- $$ language plpgsql;


