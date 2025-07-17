-- may need some more work
-- use Postgres boolean type, single interval instead of int4range
-- for OR, cap it at 1, minimum of adding the two values and 1.
CREATE OR REPLACE FUNCTION range_set_and(set1 int4range[], set2 int4range[]) RETURNS int4range[] AS $$
DECLARE
    result int4range[];
BEGIN
    result := ARRAY[]::int4range[];
    FOR i IN 1..array_length(set1, 1) LOOP
        result := array_append(result, set1[i] + set2[i]);
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;