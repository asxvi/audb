CREATE OR REPLACE FUNCTION prune_gt(set1 int4range[], k int)
RETURNS int4range[] as $$
BEGIN
    RETURN (
		SELECT array_agg(int4range(greatest(lower(i), k), upper(i)-1, '[]'))
        FROM unnest(set1) i
        WHERE upper(i) > k
	);
END;
$$ LANGUAGE plpgsql;