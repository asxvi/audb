CREATE OR REPLACE FUNCTION prune_equal(set1 int4range[], set2 int4range[], direction bool)
RETURNS int4range[] as $$
BEGIN
    RETURN (
		SELECT array_agg( int4range(lower(i), least(upper(i)-1, k), '[]') )
        FROM unnest(set1) i
        WHERE lower(i) < k
	);
END;
$$ LANGUAGE plpgsql;







CREATE OR REPLACE FUNCTION prune_equal(set1 int4range, set2 int4range, direction bool)
RETURNS int4range as $$
BEGIN
  IF direction = FALSE THEN
    RETURN int4range(upper(set2))  
  END IF;
  RETURN ()
END;
$$ LANGUAGE plpgsql;


