CREATE OR REPLACE FUNCTION sort2(ranges int4range[])
RETURNS int4range[] AS $$
DECLARE
    sorted_ranges int4range[];
BEGIN
    SELECT array_agg(vals ORDER BY lower(vals), upper(vals))
    INTO sorted_ranges
    FROM unnest(ranges) AS vals;
    
    RETURN sorted_ranges;
END;
$$ LANGUAGE plpgsql;





SELECT array_agg(vals ORDER BY lower(vals), upper(vals))
FROM unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS vals;



select array_agg(vals order by i) 
from (
	select *, lower(vals) i, upper(vals) j
	from unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS vals
) v;


select array_agg(vals) 
from (
	select *, lower(vals) i, upper(vals) j
	from unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS vals
) v;



select * from 
(
	select *
	from unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS vals
)





select * 
from unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS t1,
unnest(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]) AS t2
where t1 <> t2;


-------------
CREATE OR REPLACE FUNCTION range_distance(r1 int4range, r2 int4range)
RETURNS integer AS $$
BEGIN
    IF r1 @> r2 OR r1 <@ r2 THEN
        RETURN 0;
    ELSIF upper(r1) < lower(r2) THEN
        RETURN lower(r2) - upper(r1);
    ELSE
        RETURN lower(r1) - upper(r2);
    END IF;
END;
$$ LANGUAGE plpgsql;


select * from range_distance(int4range(-11,-7), int4range(20,200))








CREATE OR REPLACE FUNCTION reduceSize(vals int4range[], numRangesKeep int)
-- RETURNS int4range[] AS $$
RETURNS int[] AS $$
DECLARE
    normalizedRanges int4range[];
	currNumRanges int;
    distance int;
    prev int4range;
    curr int4range;
    rv int[] = '{}';

BEGIN
	currNumRanges := array_length(vals, 1);
--	IF vals IS NULL OR currNumRanges IS NULL OR numRangesKeep < 1 THEN
--        RETURN '{}';
--	ELSIF currNumRanges <= numRangesKeep THEN
--		RETURN '{}';
--		--RETURN vals;
--    END IF;

    normalizedRanges := normalize_vals(vals);
    currNumRanges := array_length(normalizedRanges, 1);

    prev := normalizedRanges[1];
    FOR i IN 2..currNumRanges LOOP
		curr := normalizedRanges[i];
        distance := abs(range_distance(prev, curr));
        rv := rv || distance; 
    END LOOP;

    RETURN rv;
END;
$$ LANGUAGE plpgsql;


select reduceSize(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)], 2);




CREATE OR REPLACE FUNCTION normalize_vals2(vals int4range[])
RETURNS int4range[] AS $$
BEGIN
    -- Sort ranges by lower bound and merge overlapping/adjacent ranges
    RETURN ARRAY(
        SELECT range_merge_agg(unnest)
        FROM unnest(vals) AS r
        ORDER BY lower(r)
    );
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION reduceSize(vals int4range[], numRangesKeep int)
--RETURNS int4range[] AS $$
RETURNS int[] AS $$
DECLARE
    normalizedRanges int4range[];
	currNumRanges int;
    distance int;
    prev int4range;
    curr int4range;
    rv int[] = '{}';

BEGIN
	currNumRanges := array_length(vals, 1);
--	IF vals IS NULL OR currNumRanges IS NULL OR numRangesKeep < 1 THEN
--        RETURN '{}';
--	ELSIF currNumRanges <= numRangesKeep THEN
--		RETURN '{}';
--		--RETURN vals;
--    END IF;

    normalizedRanges := normalize_vals(vals);
    currNumRanges := array_length(normalizedRanges, 1);

    return normalizedRanges;

    -- prev := normalizedRanges[1];
    -- FOR i IN 2..currNumRanges LOOP
	-- 	curr := normalizedRanges[i];
    --     distance := abs(range_distance(prev, curr));
    --     rv := rv || distance; 
    -- END LOOP;

    -- RETURN rv;
END;
$$ LANGUAGE plpgsql;










----------------
----------------
CREATE OR REPLACE FUNCTION range_distance(r1 int4range, r2 int4range)
RETURNS integer AS $$
BEGIN
    IF r1 @> r2 OR r1 <@ r2 THEN
        RETURN 0;
    ELSIF upper(r1) < lower(r2) THEN
        RETURN lower(r2) - upper(r1);
    ELSE
        RETURN lower(r1) - upper(r2);
    END IF;
END;
$$ LANGUAGE plpgsql;

drop function reduceSize3;

CREATE OR REPLACE FUNCTION reduceSize3(vals int4range[], numRangesKeep int)
-- RETURNS int4range[] AS $$
RETURNS text[] AS $$
DECLARE
    sortedRanges int4range[];
	currNumRanges int;
    distance int := NULL;
	index int := NULL;
    prev int4range;
    curr int4range;

BEGIN
	currNumRanges := array_length(vals, 1);
	IF vals IS NULL OR currNumRanges IS NULL OR numRangesKeep < 1 THEN
        RETURN '{}';
	ELSIF currNumRanges <= numRangesKeep THEN
		RETURN vals;
    END IF;

    sortedRanges := sort(vals);
    currNumRanges := array_length(sortedRanges, 1);
	
	WHILE currNumRanges > numRangesKeep LOOP
		prev := sortedRanges[1];
	    FOR i IN 2..currNumRanges LOOP
			curr := sortedRanges[i];
			
			-- get the smallest gap O(n) each time
			IF distance is NULL or abs(range_distance(prev, curr)) < distance THEN
				distance := range_distance(prev, curr);	
				index := i-1;
			END IF;
			prev := curr; 

	    END LOOP;
		
		sortedRanges[index] := int4range(
								least(lower(sortedRanges[index]), lower(sortedRanges[index+1])), 
								greatest(upper(sortedRanges[index]), upper(sortedRanges[index+1])) 
							);			
		
		FOR i in index+1..currNumRanges-1 LOOP
			sortedRanges[i] := sortedRanges[i+1];
		END LOOP;
		
		currNumRanges := currNumRanges-1;
		distance := NULL;	
	END LOOP;

   
    RETURN sortedRanges[1:numRangesKeep];
END;
$$ LANGUAGE plpgsql;


--select * from reducesize3(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)], 2)
--select * from reducesize3(ARRAY[int4range(10,15), int4range(1,2), int4range(30,50), int4range(17,20)], 2);
select * from unnest(
	reducesize3(ARRAY[int4range(1,2), int4range(10,15), int4range(17,20), int4range(30,50), int4range(-101,-22), int4range(110,125)], 5)
);


select * from array_length(ARRAY[int4range(1,2), int4range(10,15), int4range(17,20), int4range(30,50)], 1);


















