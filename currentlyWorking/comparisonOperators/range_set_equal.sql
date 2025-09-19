CREATE OR REPLACE FUNCTION range_set_equal(set1 int4range[], set2 int4range[])
RETURNS BOOLEAN AS $$    
BEGIN
    IF array_length(set1, 1) != array_length(set2, 1) THEN
        RETURN FALSE;
    END IF;
    
    FOR i in 1..array_length(set1, 1) LOOP
        IF lower(set1[i]) != lower(set2[i]) OR upper(set1[i]) != upper(set2[i]) THEN
            RETURN FALSE;
		END IF;
    END LOOP;

    RETURN TRUE;
END;
$$ LANGUAGE plpgsql;