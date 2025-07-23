CREATE OR REPLACE FUNCTION normalize_vals(vals int4range[])
RETURNS int4range[] AS $$
DECLARE
    rv int4range[] := '{}';
    sorted int4range[] := '{}';
    currMin int := NULL;
    currMax int := NULL;
BEGIN
    sorted := lsort2(vals);

	-- init min/max to first int4range
	currMin := lower(sorted[1]);
    currMax := upper(sorted[1]);

    -- iter once and keep track of local/global min/mxa
    for i in 2..array_length(sorted, 1) LOOP
		IF currMax >= lower(sorted[i]) then
            currMax := upper(sorted[i]);
		ELSE 
            rv := array_append(rv, int4range(currMin, currMax));
            currMin := lower(sorted[i]);
            currMax := upper(sorted[i]);
		end if;
	end loop;
	
    -- last iteration doesn't acccount for last range
	rv := array_append(rv, int4range(currMin, currMax));
    return rv;
END;
$$ LANGUAGE plpgsql;