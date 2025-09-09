-- find all intersections between 2 sets and normalize to fewest/smallest ranges
-- not sure if calling normalize is okay
CREATE OR REPLACE FUNCTION range_set_and(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN '{}';
    END IF;

    -- return only non empty or non NULL ranges
    RETURN normalize_vals(
        ARRAY(
            SELECT i * j
            FROM unnest(set1) AS i, unnest(set2) AS j
            WHERE (i * j) IS NOT NULL AND NOT isempty(i * j)
        )
    );
END;
$$ LANGUAGE plpgsql;