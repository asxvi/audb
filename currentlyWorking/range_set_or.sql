-- find all unions between 2 sets and normalize to fewest/smallest ranges
-- not sure if calling normalize is okay???
CREATE OR REPLACE FUNCTION range_set_or(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL AND set2 IS NULL THEN
        RETURN '{}';
    ELSIF set1 IS NULL THEN
        -- RETURN normalize_vals(set2);
        RETURN set2;
    ELSIF set2 IS NULL THEN
        -- RETURN normalize_vals(set1);
        RETURN set1;
    END IF;

    --    return nomalized values or no?
    -- RETURN normalize_vals(set1 || set2);
    RETURN (set1 || set2);
END;
$$ LANGUAGE plpgsql;


-- SELECT (range_set_or(
--         ARRAY[int4range(1,4), int4range(6,9), int4range(2,5)],
--         ARRAY[int4range(100,1220), int4range(3, 6)]
--     )) as jack;