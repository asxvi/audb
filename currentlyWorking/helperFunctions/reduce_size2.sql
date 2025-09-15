CREATE OR REPLACE FUNCTION reduceSize(vals int4range[], numRangesKeep int)
-- RETURNS int4range[] AS $$
RETURNS int[] AS $$
DECLARE
    normalizedRanges int4range[];
	currNumRanges int;
    distance int;
    prev int4range;
    curr int4range;
    rv int[];

BEGIN
	currNumRanges := array_length(vals, 1)
	IF vals IS NULL OR currNumRanges IS NULL OR numRangesKeep < 1 THEN
        RETURN '{}';
	ELSIF currNumRanges <= numRangesKeep THEN
		RETURN vals;
    END IF;

    normalizedRanges := normalize_vals(vals);
    currNumRanges := array_length(normalizedRanges, 1);


    prev := normalizedRanges[1];
    FOR i IN 2..currNumRanges LOOP
        distance := abs(range_distance(prev, curr))
        distance || rv; 
    END LOOP;

    RETURN rv;
END;
$$ LANGUAGE plpgsql;

-- result: does postgres naturally smush to 1d array?
--         query         | pg_typeof | query 
-- ----------------------+-----------+-------
--  {{1,-1},{2,1},{3,0}} | integer[] |      
select query, pg_typeof(query), query[1]
from reduceSize(ARRAY[int4range(10,24), int4range(2,5), int4range(1,3), int4range(6,10)], 3) as query;





-- array_dims ( anyarray ) → text
-- Returns a text representation of the array's dimensions.
-- array_dims(ARRAY[[1,2,3], [4,5,6]]) → [1:2][1:3]
