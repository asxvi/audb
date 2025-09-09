CREATE OR REPLACE FUNCTION range_add(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN 
        int4range(lower(set1) + lower(set2), 
        upper(set1) + upper(set2),
        '[]'
    );
END;
$$ LANGUAGE plpgsql;