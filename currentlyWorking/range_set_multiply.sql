-- take cartesian product and returns largest range possible
CREATE OR REPLACE FUNCTION range_set_multiply(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF array_length(set1, 1) IS NULL THEN
        RETURN set2;
    END IF;

    IF array_length(set2, 1) IS NULL THEN
        RETURN set1;
    END IF;

    RETURN ARRAY(
        SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4) + 1)
        FROM (
            SELECT 
                lower(i) * lower(j) as p1,
                lower(i) * upper(j) as p2,
                upper(i) * lower(j) as p3,
                upper(i) * upper(j) as p4
            FROM unnest(set1) i, unnest(set2) j
        ) calc
    );
END;
$$ LANGUAGE plpgsql;


-- SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4) + 1)
-- FROM (
--     SELECT 
--         lower(i) * lower(j) as p1,
--         lower(i) * upper(j) as p2,
--         upper(i) * lower(j) as p3,
--         upper(i) * upper(j) as p4
--     FROM UNNEST(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i
--     CROSS JOIN unnest(ARRAY[int4range(2,3), int4range(5,9)]) j
-- ) alex;

