CREATE OR REPLACE FUNCTION normalize_vals(vals int4range[])
RETURNS int4range[] AS $$
DECLARE
    rv int4range[] := '{}';
    sorted int4range[] := '{}';
    currMin int := NULL;
    currMax int INT := NULL;
BEGIN
    IF vals IS NULL OR array_length(vals, 1) IS NULL THEN
        RETURN '{}';
    END IF;

    sorted := lsort2(vals);

	-- init min/max to first int4range
	currMin := lower(sorted[1]);
    currMax := upper(sorted[1]);

    -- iter once and keep track of local/global min/mxa
    FOR i IN 2..array_length(sorted, 1) LOOP
		IF currMax >= lower(sorted[i]) THEN
            currMax := upper(sorted[i]);
		ELSE 
            rv := array_append(rv, int4range(currMin, currMax));
            currMin := lower(sorted[i]);
            currMax := upper(sorted[i]);
		END IF;
	END LOOP;
	
    -- last iteration doesn't acccount for last range
	rv := array_append(rv, int4range(currMin, currMax));
    RETURN rv;
END;
$$ LANGUAGE plpgsql;