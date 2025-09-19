-- for every () in x
--     find min and find max 
-- for every () in y
--     find min and find max
-- then just do basic 3vl comparison 
-- O(n+m)

-- assume that parameters are not normalized. If want to normalize, call normalize, and then just index at set[1], set[last]
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
    IF array_length(set1, 1) IS NULL OR array_length(set2, 1) IS NULL THEN
        RETURN NULL;
    END IF;

    -- find absolute min and absolute max in set2
    FOR i IN (SELECT unnest(set1)) LOOP
        IF min1 IS NULL OR lower(i) < min1 THEN
            min1 := lower(i);
        END IF;

        IF max1 IS NULL OR upper(i) > max1 THEN
            max1 := upper(i);
        END IF;
    END LOOP;

    FOR j IN (SELECT unnest(set2)) LOOP
        IF min2 IS NULL OR lower(j) < min2 THEN 
            min2 := lower(j);
        END IF;

        IF max2 IS NULL OR upper(j) > max2 THEN
            max2 := upper(j);
        END IF;
    END LOOP;

    -- looking for disjoint 
    IF max1 < min2 THEN
        RETURN False;
    elsif min1 < max2 THEN 
        RETURN NULL;
    else 
        RETURN TRUE;
    end if;

END;
$$ LANGUAGE plpgsql;