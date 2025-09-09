--TEST INITIAL STUFF FROM MONTH AGO

-- compute Minowski sum, O(n x m). Assume exclusive upper bound
CREATE OR REPLACE FUNCTION range_set_add(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN ARRAY(
        SELECT int4range((lower(i) + lower(j)), (upper(i) + upper(j)), '[]')
        FROM unnest(set1) i, unnest(set2) j
    );
END;
$$ LANGUAGE plpgsql;



------------TEST CASES

 SELECT (range_set_add(
   ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)],
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 SELECT (range_set_add(
   '{}',
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 SELECT (range_set_add(
   NULL,
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 
 SELECT (range_add(
   int4range(1,2),
   int4range(2,3)
));

SELECT (range_add(
   NULL,
   int4range(2,3)
));
 
------------TEST CASES
 
 
 
 --CHANGE STUFF UP, INCLUSIVE RESULT AND REMOVE BS +1 -1 all that 
 -- compute Minowski sum, O(n x m). Assume exclusive upper bound
CREATE OR REPLACE FUNCTION range_set_add(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;

    RETURN ARRAY(
        SELECT int4range(lower(i) + lower(j), upper(i) + upper(j), '[]')
        FROM unnest(set1) i, unnest(set2) j
    );
END;
$$ LANGUAGE plpgsql;



--SELECT int4range(lower(i) + lower(j), upper(i) + upper(j), '[]')
select *
FROM unnest(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i, unnest(ARRAY[int4range(2,3), int4range(5,9)]) j



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








CREATE OR REPLACE FUNCTION range_subtract(set1 int4range, set2 int4range)
RETURNS int4range AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
    END IF;
    
    RETURN int4range(
        lower(set1) - upper(set2),
        upper(set1) - lower(set2),
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
            SELECT (lower(i) - upper(j)) as l, 
                   (upper(i) - lower(j)) as u
            FROM unnest(set1) i, unnest(set2) j
        ) calc
        WHERE l < u
    );
END;
$$ LANGUAGE plpgsql;




SELECT (range_set_subtract(
   ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)],
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 SELECT (range_set_subtract(
   '{}',
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 SELECT (range_set_subtract(
   NULL,
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
 
 SELECT (range_subtract(
   int4range(1,2),
   int4range(2,3)
));

SELECT (range_subtract(
   NULL,
   int4range(2,3)
));








