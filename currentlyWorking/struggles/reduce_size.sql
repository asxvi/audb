CREATE OR REPLACE FUNCTION reduceSize(vals int4range[], numRangesKeep int)
-- RETURNS int4range[] AS $$
RETURNS int[] AS $$
DECLARE
    rangesSorted int4range[];
	merged_positions int[] := '{}';
	indexDist integer[][];
	rv integer[][];
	currNumRanges int;
	gap_ordered RECORD[];
BEGIN
	IF vals IS NULL OR array_length(vals, 1) IS NULL OR numRangesKeep < 1 THEN
        RETURN '{}';
    END IF;

	currNumRanges := array_length(vals, 1)
	IF currNumRanges <= numRangesKeep THEN
        RETURN vals;
    END IF;

    rangesSorted := sort(vals);

    FOR i IN 1..currNumRanges -1 LOOP
		-- indexDist := indexDist || ARRAY[[i, lower(rangesSorted[i+1]) - upper(rangesSorted[i])]];
		indexDist := array_append(indexDist, ROW(i, lower(sorted_ranges[i+1]) - upper(sorted_ranges[i]))::RECORD);
    END LOOP;
	
	-- order by gap size smallest->largest
	SELECT array_agg(temp) into gap_ordered
	FROM (
		SELECT temp
		FROM unnest(indexDist) as temp(idx int, gap int)
		ORDER BY temp.gap
	) as sorted_gaps;
	
	FOR i IN 1 .. (currNumRanges - numRangesKeep) LOOP
		merged_positions := array_append(merged_positions, gap_ordered[i].idx);
	END LOOP;

	SELECT array_agg(t order by t) INTO merged_positions
	FROM unnest(merged_positions) as t;

	return merged_positions

	-- select array_agg(tempTable order by tempTable[2]) 
	-- into rv 
	-- from unnest(indexDist) as tempTable;

    -- RETURN indexDist;
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
