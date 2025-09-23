CREATE OR REPLACE FUNCTION range_normalize(rangeset int4range[])
RETURNS int4range[] AS $$
DECLARE
    result int4range[] := '{}';
    sorted_ranges int4range[] := '{}';
    range_record int4range;
    pos int;
    current_min int;
    current_max int;
BEGIN
    -- Edge case: return empty if input is empty
    IF array_length(rangeset, 1) IS NULL THEN
        RETURN result;
    END IF;

    -- Sort ranges by start value
    sorted_ranges := lsort(rangeset);

    -- Initialize with the first range
    current_min := lower(sorted_ranges[1]);
    current_max := upper(sorted_ranges[1]);

    -- Iterate through sorted ranges and merge overlapping ones
    FOR i IN array_lower(sorted_ranges, 1)..array_upper(sorted_ranges, 1) LOOP
      range_record := sorted_ranges[i];
        IF lower(range_record) <= current_max THEN
            current_max := GREATEST(current_max, upper(range_record));
        ELSE
          result := array_append(result, int4range(current_min, current_max));
            current_min := lower(range_record);
            current_max := upper(range_record);
        END IF;
    END LOOP;

    -- Append the last range
    result := array_append(result, int4range(current_min, current_max));

    RETURN result;
END;
$$ LANGUAGE plpgsql;
