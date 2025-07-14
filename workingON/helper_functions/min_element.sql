CREATE OR REPLACE FUNCTION lower_bound(set1 int4range, set2 int4range)
RETURNS int AS $$
BEGIN
    RETURN 
        GREATEST(upper(set1), upper(set2)) -1;
END;
$$ LANGUAGE plpgsql;


-- -- 
-- SELECT lower_bound('[1,7]'::int4range, '[3,4]'::int4range) AS test1;
-- SELECT lower_bound('empty'::int4range, '[3,9]'::int4range) AS test2;
-- SELECT lower_bound(NULL::int4range, '[3,9]'::int4range) AS test3;
-- SELECT lower_bound(NULL::int4range, NULL::int4range) AS test4;
-- SELECT lower_bound('empty'::int4range, 'empty'::int4range) AS test5; -- maybe rv 'empty' and not null