-- ideally dont want to use array_append bc its expensive inside for loop
CREATE OR REPLACE FUNCTION normalize_vals(vals int4range[])
RETURNS int4range[] AS $$
DECLARE
    rv int4range[] := '{}';
    sorted int4range[] := '{}';
    prevMin int := NULL;
    prevMax int := NULL;
    currMin int := NULL;
    currMax int := NULL;
BEGIN
    IF vals IS NULL OR array_length(vals, 1) IS NULL THEN
        RETURN '{}';
    END IF;

    sorted := lsort2(vals);
    
	-- init min/max to first int4range
	prevMin := lower(sorted[1]);
    prevMax := upper(sorted[1]);

    -- iter once and keep track of local/global min/mxa
    FOR i IN 2..array_length(sorted, 1) LOOP
        currMin := lower(sorted[i]);
        currMax := upper(sorted[i]);

        -- check if range is within, then dont update max
		IF currMin < prevMax THEN
            IF currMax < prevMin THEN
                prevMax := prevMax;
            ELSE
                prevMax := currMax;
            END IF;
		-- range 
        ELSE   
            rv := array_append(rv, int4range(prevMin, prevMax));
            prevMin := lower(sorted[i]);
            prevMax := upper(sorted[i]);
		END IF;
	END LOOP;
	
    -- last iteration doesn't acccount for last range
	rv := array_append(rv, int4range(prevMin, currMax));
    RETURN rv;
END;
$$ LANGUAGE plpgsql;




select int4range(1, 7) && int4range(2,5)


select * 
from normalize_vals(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]);





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
    FOR i in 2..array_length(sorted, 1) LOOP
		curr := sorted[i];
		
		-- if the ranges overlap, check direction of overlap		
		IF prev && curr THEN
			prev := int4range(least(lower(prev), lower(curr)), greatest(upper(prev), upper(curr)) );			
		ELSE
			--rv := array_append(rv, prev);
			rv := rv || prev;
			prev := curr;
		END IF;
				
        --rv := array_append(rv, prev);
    END LOOP;
    	rv := rv || prev;
    return rv;
END;
$$ LANGUAGE plpgsql;



select * from normalize_vals(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]);
-- overlap case
select * from normalize_vals(ARRAY[int4range(-11,-7), int4range(-25,2)]);

--no overlap
select * from normalize_vals(ARRAY[int4range(-11,-7), int4range(6,10)]);


select * from sort(ARRAY[int4range(-11,-7), int4range(-25,2), int4range(-11,26), int4range(6,7)]);





