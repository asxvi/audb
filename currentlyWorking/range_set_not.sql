CREATE OR REPLACE FUNCTION range_set_and(set1 int4range[], set2 int4range[]) RETURNS int4range[] AS $$
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