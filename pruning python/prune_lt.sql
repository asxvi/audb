CREATE OR REPLACE FUNCTION prune_lt(set1 int4range[], k int)
RETURNS int4range[] as $$
BEGIN
    RETURN (
		SELECT array_agg( int4range(lower(i), least(upper(i)-1, k), '[]') )
        FROM unnest(set1) i
        WHERE lower(i) < k
	);
END;
$$ LANGUAGE plpgsql;






CREATE OR REPLACE FUNCTION prune_lt(set1 int4range[], set2 int4range[], direction bool)
RETURNS int4range[] as $$
BEGIN
    RETURN (
		SELECT array_agg( int4range(lower(i), least(upper(i)-1, k), '[]') )
        FROM unnest(set1) i
        WHERE lower(i) < k
	);
END;
$$ LANGUAGE plpgsql;


[(1,3), (4,5)] < [(2,4), (1,7)]


1,3 < 2,4 NULL     NULL  1,3 < 1,7
4,5 < 2,4 False    NULL  4,5 < 1,7



prune_lt([(1,4)], [(2,2)], 0=left)
left == (1,2)

prune_lt([(1,4)], [(2,2)], 1=right)
right == (2,4)



