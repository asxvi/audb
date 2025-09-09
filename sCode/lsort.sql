CREATE OR REPLACE FUNCTION lsort(ranges int4range[])
RETURNS int4range[] AS $$
DECLARE
    sorted_ranges int4range[];
BEGIN
    SELECT array_agg(rng ORDER BY lower(rng), upper(rng))
    INTO sorted_ranges
    FROM unnest(ranges) AS rng;
    
    RETURN sorted_ranges;
END;
$$ LANGUAGE plpgsql;