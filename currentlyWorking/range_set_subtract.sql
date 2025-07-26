-- i think this works, test for none types or nulls
CREATE OR REPLACE FUNCTION range_set_subtract(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
BEGIN
    IF set1 IS NULL OR set2 IS NULL THEN
        RETURN '{}';
    END IF;

    RETURN ARRAY(
        SELECT (int4range((lower(i) - upper(j)-1), (upper(i)-1 - lower(j))))
        FROM unnest(set1) i, unnest(set2) j
    );
    -- RETURN result;
END;
$$ LANGUAGE plpgsql;



 SELECT (range_set_subtract(
   ARRAY[int4range(1,4), int4range(3,6), int4range(6,8)],
   ARRAY[int4range(2,3), int4range(5,9)]
 ));


--  SELECT (range_set_subtract(
--    ARRAY[int4range(1,4), int4range(6,9)],
--    ARRAY[int4range(2,5), int4range(4,8)]
--  ));

--  SELECT (range_set_subtract(
--    ARRAY[int4range(1,4)],
--    ARRAY[int4range(2,5)]
--  ));
