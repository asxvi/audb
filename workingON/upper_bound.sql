CREATE OR REPLACE FUNCTION upper_bound(set1 int4range, set2 int4range)
RETURNS int AS $$
BEGIN
    RETURN 
        GREATEST(upper(set1), upper(set2)) -1;
END;
$$ LANGUAGE plpgsql;


-- -- 
-- SELECT upper_bound('[1,7]'::int4range, '[3,4]'::int4range) AS test1;
-- SELECT upper_bound('empty'::int4range, '[3,9]'::int4range) AS test2;
-- SELECT upper_bound(NULL::int4range, '[3,9]'::int4range) AS test3;
-- SELECT upper_bound(NULL::int4range, NULL::int4range) AS test4;
-- SELECT upper_bound('empty'::int4range, 'empty'::int4range) AS test5; -- maybe rv 'empty' and not null