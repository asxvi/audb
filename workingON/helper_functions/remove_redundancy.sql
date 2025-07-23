CREATE OR REPLACE FUNCTION remove_redundancy(ranges int4range[])
RETURNS int4range[] AS $$
DECLARE
    sorted int4range[] := range_sort(ranges); 
    result int4range[] := '{}';
    current int4range;
    last int4range;
    result_len int;


BEGIN
    FOREACH current IN ARRAY sorted LOOP
        -- If result is empty, add the first range
        IF array_length(result, 1) IS NULL THEN
            result := ARRAY[current];
        ELSE
            -- Get the last range in the result
            result_len := array_upper(result, 1);
            last := result[result_len];

            -- If current range overlaps with last, merge them
            IF upper(last) >= lower(current) THEN
                result[result_len] := int4range(
                    lower(last),
                    GREATEST(upper(last), upper(current))
                );

            -- If current range does not overlap with last, append it
            ELSE
                result := array_append(result, current);
            END IF;
        END IF;
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;
