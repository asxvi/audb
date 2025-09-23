--------------------------------
--FIX OVERAPPROX ON UPPERBOUND--


-----add-----

CREATE OR REPLACE FUNCTION set_add(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN 
        int4range(lower(set1) + lower(set2), 
        upper(set1)-1 + upper(set2)-1,          --subtract 2 bc exclusive upper bound
        '[]'
    );
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION range_set_add(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN ARRAY(
        SELECT int4range(lower(i) + lower(j), upper(i)-1 + upper(j)-1, '[]')
        FROM unnest(set1) i, unnest(set2) j
    );
END;
$$ LANGUAGE plpgsql;

select set_add(int4range(2,3), int4range(3,4));
select range_set_add(array[int4range(2,3), int4range(5,7)], array[int4range(3,4)]);

-----add-----


-----sub-----

CREATE OR REPLACE FUNCTION range_subtract(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;
    
    RETURN int4range(
        lower(set1) - (upper(set2)-1),
        (upper(set1)-1) - lower(set2),
		'[]'
    );
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION range_set_subtract(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    -- want to return null, bc its a valid range value; in every world, this val is null
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;
    
    RETURN ARRAY(
        SELECT int4range(l, u, '[]')
        FROM (
            SELECT (lower(i) - (upper(j)-1)) as l, 
                   ((upper(i)-1) - lower(j)) as u
            FROM unnest(set1) i, unnest(set2) j
        ) calc
        WHERE l <= u
    );
END;
$$ LANGUAGE plpgsql;


select range_subtract(int4range(2,3), int4range(3,4));
-- [2,2] - [3,3] = [-1, -1] = [-1, 0)
select range_set_subtract(array[int4range(2,3), int4range(5,7)], array[int4range(3,4)]);
-- ([2,2], [5,6]) - [3,3] = ([2-3, 2-3], [5-3, 6-3]) == ([-1, -1], [2, 3]) == [[-1, 0), [2, 4)]

-----sub-----


-----mul-----

CREATE OR REPLACE FUNCTION set_multiply(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN (
        SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4), '[]')
        FROM (
            SELECT 
                lower(set1) * lower(set2) as p1,
                lower(set1) * (upper(set2)-1) as p2,
                (upper(set1)-1) * lower(set2) as p3,
                (upper(set1)-1) * (upper(set2)-1) as p4
        ) calc
    );
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION range_set_multiply(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN ARRAY(
        SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4), '[]')
        FROM (
            SELECT 
                lower(i) * lower(j) as p1,
                lower(i) * (upper(j)-1) as p2,
                (upper(i)-1) * lower(j) as p3,
                (upper(i)-1) * (upper(j)-1) as p4
            FROM unnest(set1) i, unnest(set2) j
        ) calc
    );
END;
$$ LANGUAGE plpgsql;

select set_multiply(int4range(2,4), int4range(2,4));
-- [2,3] * [2,3] = [4, 9] = [4,10) 
select range_set_multiply(array[int4range(2,3), int4range(5,7)], array[int4range(3,4)]);
-- ([2,2], [5, 6]) * ([3,3]) = ([2*3, 2*3], [5*3, 6*3]) == ([6,6], [15,18]) == ([6,7), [15,19))

-----mul-----

-----div-----

CREATE OR REPLACE FUNCTION set_divide(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN (
        SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4), '[]')
        FROM (
            SELECT 
                lower(set1) / lower(set2) as p1,
                lower(set1) / (upper(set2)-1) as p2,
                (upper(set1)-1) / lower(set2) as p3,
                (upper(set1)-1) / (upper(set2)-1) as p4
        ) calc
        -- ignore bounds that cross 0 because this is not possible to divide by.
        -- ignore [0,1) == [0,0]
        WHERE NOT(lower(set2) <= 0 AND upper(set2) > 0) 
          AND NOT (lower(set2) = 0 AND upper(set2) = 1)
    );
END;
$$ LANGUAGE plpgsql;

-- not sure if this works as expected. Still confused bc div returns fraction
CREATE OR REPLACE FUNCTION range_set_divide(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF array_length(set1, 1) IS NULL THEN
        RETURN set2;
    END IF;
    IF array_length(set2, 1) IS NULL THEN
        RETURN set1;
    END IF;

    RETURN ARRAY(
        SELECT int4range(
            ((LEAST(p1, p2, p3, p4)))::int, 
            ((GREATEST(p1, p2, p3, p4)) + 1)::int
        )
        FROM (
            SELECT 
                lower(i) / lower(j) as p1,
                lower(i) / (upper(j)-1) as p2,
                (upper(i)-1) / lower(j) as p3,
                (upper(i)-1) / (upper(j)-1) as p4
            FROM unnest(set1) i, unnest(set2) j
            -- ignore bounds that cross 0 because this is not possible to divide by.
			-- ignore [0,1) == [0,0]
            WHERE NOT(lower(j) <= 0 AND upper(j) > 0) 
			  AND NOT (lower(j) = 0 AND upper(j) = 1)
        ) calc
    );
END;
$$ LANGUAGE plpgsql;

select set_divide(int4range(10,21), int4range(-1,3));
select range_set_divide(array[int4range(10,21), int4range(30,41)], array[int4range(2,5)]);


select (10/4), div(10,4)
-----div-----
