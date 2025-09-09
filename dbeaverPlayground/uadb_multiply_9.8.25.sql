-- take cartesian product and returns largest range possible
CREATE OR REPLACE FUNCTION range_set_multiply(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN NULL;
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



--MODIFY AND CHECK NOW
-- take cartesian product and returns largest range possible
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
                lower(i) * upper(j) as p2,
                upper(i) * lower(j) as p3,
                upper(i) * upper(j) as p4
            FROM unnest(set1) i, unnest(set2) j
        ) calc
    );
END;
$$ LANGUAGE plpgsql;


--------------TEST CASES
SELECT (range_set_multiply(
   ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)],
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
SELECT (range_set_multiply(
   ARRAY[int4range(-10,-4), int4range(-3,6), int4range(6,8)],
   ARRAY[int4range(-12,-3), int4range(-5,9), int4range(3, 10)]
 ));
SELECT (range_set_multiply(
   '{}',
   ARRAY[int4range(2,3), int4range(5,9)]
 ));
SELECT (range_set_multiply(
   NULL,
   ARRAY[int4range(2,3), int4range(5,9)]
 ));


SELECT (set_multiply(
   int4range(1,4),
   int4range(2,3)
 ));
SELECT (set_multiply(
   int4range(-10,-4),
   int4range(-12,-3)
 ));
SELECT (set_multiply(
   '{}',
   int4range(2,3)
 ));
SELECT (set_multiply(
   NULL,
   int4range(2,3)
 ));

--------------TEST CASES

SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4), '[]'), i, j
        FROM (
            SELECT 
                lower(i) * lower(j) as p1,
                lower(i) * upper(j) as p2,
                upper(i) * lower(j) as p3,
                upper(i) * upper(j) as p4, 
                i,j
            FROM unnest(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i, unnest(ARRAY[int4range(2,3), int4range(5,9)]) j
        ) calc
        
 
SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4), '[]'), i, j
        FROM (
            SELECT 
                lower(i) * lower(j) as p1,
                lower(i) * upper(j) as p2,
                upper(i) * lower(j) as p3,
                upper(i) * upper(j) as p4, 
                i,j
            FROM unnest(ARRAY[int4range(-10,-4), int4range(-3,6), int4range(6,8)]) i, unnest(ARRAY[int4range(-2,3), int4range(-5,9)]) j
        ) calc
        
        
        
        
 SELECT int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4) + 1)
 FROM (
     SELECT 
         lower(i) * lower(j) as p1,
         lower(i) * upper(j) as p2,
         upper(i) * lower(j) as p3,
         upper(i) * upper(j) as p4
     FROM UNNEST(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i
     CROSS JOIN unnest(ARRAY[int4range(2,3), int4range(5,9)]) j
 ) alex;

 
 select * FROM UNNEST(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i
     CROSS JOIN unnest(ARRAY[int4range(2,3), int4range(5,9)]) j

select * from UNNEST(ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)]) i, unnest(ARRAY[int4range(2,3), int4range(5,9)]) j










-- take cartesian product and returns largest range possible
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
                lower(set1) * upper(set2) as p2,
                upper(set1) * lower(set2) as p3,
                upper(set1) * upper(set2) as p4
        ) calc
    );
END;
$$ LANGUAGE plpgsql;
