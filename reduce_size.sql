CREATE OR REPLACE FUNCTION reduce_size(rangeset int4range[], max_inter int) RETURNS int4range[] AS $$
DECLARE
    result int4range[];
BEGIN
    -- Sort the rangeset by lower bounds
    result := ARRAY(SELECT UNNEST(rangeset) ORDER BY LOWER(UNNEST(rangeset)));
    -- Return only the first max_inter intervals
    RETURN result[:max_inter];
END;
$$ LANGUAGE plpgsql;

-- optimal solution: try to merge all intervals until you get the desired number.