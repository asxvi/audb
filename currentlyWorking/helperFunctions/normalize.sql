CREATE OR REPLACE FUNCTION normalize_vals(vals int4range[])
RETURNS int4range[] AS $$
DECLARE
    rv int4range[] := '{}';
    sorted int4range[] := '{}';
    prev int4range;
    curr int4range;
BEGIN
	IF vals IS NULL OR array_length(vals, 1) IS NULL THEN
        RETURN '{}';
    END IF;

    sorted := sort(vals);
    prev := sorted[1];

    FOR i IN 2..array_length(sorted, 1) LOOP
		curr := sorted[i];
		
		-- if the ranges overlap, then keep largest possible range
		IF prev && curr THEN
			prev := int4range(least(lower(prev), lower(curr)), greatest(upper(prev), upper(curr)) );			
		-- otherwise append prev range, and set prev to curr
        ELSE
			rv := rv || prev;
			prev := curr;
		END IF;
    END LOOP;
    	rv := rv || prev;
    return rv;
END;
$$ LANGUAGE plpgsql;



CREATE OR REPLACE FUNCTION reduceSize(vals int4range[], numRangesKeep int)
RETURNS int4range[] AS $$
-- RETURNS int[] AS $$
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