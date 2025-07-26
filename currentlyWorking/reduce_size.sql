CREATE OR REPLACE FUNCTION sort(ranges int4range[], numRangesKeep int)
RETURNS int4range[] AS $$
DECLARE
    rangesSorted int4range[] := '{}';
    alex string := "";
BEGIN
    rangesSorted := lsort(ranges);
    -- FOR i IN 1..array_length(set1, 1) LOOP
    FOR i IN 1..array_length(rangesSorted, 1) LOOP
        alex || i;
    END LOOP;



    RETURN alex;
END;
$$ LANGUAGE plpgsql;