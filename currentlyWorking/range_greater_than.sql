CREATE OR REPLACE FUNCTION range_greater_than(set1 int4range[], set2 int4range[])
RETURNS boolean AS $$
DECLARE
    min1 int := NULL;
    max1 int := NULL;
    min2 int := NULL;
    max2 int := NULL;
    i int4range;
    j int4range;
BEGIN 
    -- check for empty sets 
    if array_length(set1, 1) is NULL or array_length(set2, 1) is NULL THEN
        RETURN NULL;
    end if;

    -- find absolute min and absolute max in set2
    FOR i IN (SELECT unnest(set1)) LOOP
        IF min1 is null or lower(i) < min1 then 
            min1 := lower(i);
        end IF;

        if max1 is null or upper(i) > max1 then 
            max1 := upper(i);
        end if;
    END LOOP;

    FOR j IN (SELECT unnest(set2)) LOOP
        IF min2 is null or lower(j) < min2 then 
            min2 := lower(j);
        end IF;

        if max2 is null or upper(j) > max2 then
            max2 := upper(j);
        end if;
    END LOOP;

    if max1 < min2 then 
        RETURN FALSE;
    elsif min1 < max2 THEN 
        RETURN NULL;
    else 
        RETURN TRUE;
    end if;

END;
$$ LANGUAGE plpgsql;

-- for every () in x
--     find min and find max 
-- for every () in y
--     find min and find max

-- then just do basic comparison 

-- O(2n)

