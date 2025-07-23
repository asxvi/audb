CREATE OR REPLACE FUNCTION range_sort(ranges int4range[])
RETURNS int4range[] AS $$
DECLARE
    r int4range;
    sorted int4range[];
BEGIN
    CREATE TEMP TABLE temporary_ranges (r int4range) ON COMMIT DROP;

    FOREACH r IN ARRAY ranges LOOP
        INSERT INTO temporary_ranges VALUES (r);
    END LOOP;

    -- Collects ranges into a sorted array using ARRAY_AGG. 
    SELECT ARRAY_AGG(r ORDER BY lower(r), upper(r)) INTO sorted FROM temporary_ranges;

    RETURN sorted;
END;
$$ LANGUAGE plpgsql;