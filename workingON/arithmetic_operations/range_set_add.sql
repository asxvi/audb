-- previous implementation returned "(,)" when params were NULL and/or 'empty'
CREATE OR REPLACE FUNCTION range_set_add(set1 int4range, set2 int4range) 
RETURNS int4range AS $$
BEGIN
    -- NULL or invalid type
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;
    
    -- empty range
    IF isempty(set1) OR isempty(set2) THEN
        RETURN 'empty'::int4range;
    END IF;
    
    RETURN int4range(
        lower(set1) + lower(set2),
        (upper(set1)-1) + (upper(set2)-1) +1 
    );
END;
$$ LANGUAGE plpgsql;


-- -- test conditions
-- SELECT range_set_add('[1,7]'::int4range, '[3,4]'::int4range) AS test1;
-- SELECT range_set_add('empty'::int4range, '[3,9]'::int4range) AS test2;
-- SELECT range_set_add(NULL::int4range, '[3,9]'::int4range) AS test3;
