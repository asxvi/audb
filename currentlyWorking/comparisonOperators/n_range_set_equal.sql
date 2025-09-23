CREATE OR REPLACE FUNCTION range_set_equal(set1 int4range[], set2 int4range[])
RETURNS BOOLEAN AS $$   
DECLARE 
    norm1 int4range[] := '{}';
    norm2 int4range[] := '{}';
    numPossible int := 0;
BEGIN
    norm1 := normalize_vals(set1);
    norm2 := normalize_vals(set2);

    IF array_length(norm1,1) IS NULL AND array_length(norm2,1) IS NULL THEN
        RETURN TRUE;
    END IF;
    IF array_length(norm1,1) IS NULL OR array_length(norm2,1) IS NULL THEN
        RETURN FALSE;
    END IF;

	-- vacuously true if a=b=c=d
    IF array_length(norm1,1) = 1 AND array_length(norm1,1) = array_length(norm2,1) THEN
        if (lower(norm1[1]) = upper(norm1[1])-1
			AND upper(norm1[1])-1 = lower(norm2[1])
			AND lower(norm2[1]) = upper(norm2[1])-1) THEN
			RETURN TRUE;
		END IF;
    END IF;
    

    -- only after normalizing the lengths shouldnt differ
    IF array_length(norm1, 1) != array_length(norm2, 1) THEN
        RETURN FALSE;
    END IF;
	
    -- go thru every range, and check if they can poissibly overlap == NULL rv
    FOR i in 1..array_length(norm1, 1) LOOP
        IF (norm1[i] = norm2[i]) OR (norm1[i] && norm2[i]);
            numPossible := numPossible + 1;
        END IF;
    END LOOP;

    IF numPossible = array_length(norm1, 1) THEN
        RETURN NULL;
    END IF;

    RETURN FALSE;
END;
$$ LANGUAGE plpgsql;