-- not sure if this works as expected. Still confused bc div returns fraction
CREATE OR REPLACE FUNCTION range_set_divide(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN ARRAY(
        SELECT int4range(
            ((LEAST(p1, p2, p3, p4)))::int, 
            ((GREATEST(p1, p2, p3, p4)) + 1)::int
        )
        FROM (
            SELECT 
                -- do i do int division?
                lower(i) / lower(j) as p1,
                lower(i) / upper(j) as p2,
                upper(i) / lower(j) as p3,
                upper(i) / upper(j) as p4

                -- lower(i)::numeric / lower(j)::numeric as p1,
                -- lower(i)::numeric / upper(j)::numeric as p2,
                -- upper(i)::numeric / lower(j)::numeric as p3,
                -- upper(i)::numeric / upper(j)::numeric as p4
                
            FROM unnest(set1) i, unnest(set2) j
            -- ingnore bounds that cross 0 because this is not possible to divide by
            -- WHERE NOT(lower(j) <= 0 AND upper(j) > 0)
            WHERE (lower(j) = 0 AND upper(j) = 0)
        ) calc
    );
END;
$$ LANGUAGE plpgsql;




SELECT 
-- do i do int division?
lower(i) / lower(j) as p1,
lower(i) / upper(j) as p2,
upper(i) / lower(j) as p3,
upper(i) / upper(j) as p4

-- lower(i)::numeric / lower(j)::numeric as p1,
-- lower(i)::numeric / upper(j)::numeric as p2,
-- upper(i)::numeric / lower(j)::numeric as p3,
-- upper(i)::numeric / upper(j)::numeric as p4
    
FROM unnest(ARRAY[int4range(-10,-4), int4range(-3,6), int4range(6,8)]) i, unnest(ARRAY[int4range(-2,3), int4range(-5,9)]) j
-- ingnore bounds that cross 0 because this is not possible to divide by
-- WHERE NOT(lower(j) <= 0 AND upper(j) > 0)
WHERE (lower(j) = 0 AND upper(j) = 0)
