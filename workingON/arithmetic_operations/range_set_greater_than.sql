CREATE OR REPLACE FUNCTION range_greater_than(r1 int4range, r2 int4range)
RETURNS boolean AS $$
DECLARE
    a int := lower(r1);
    b int := upper(r1);
    c int := lower(r2);
    d int := upper(r2);
BEGIN
    IF a > d THEN
        RETURN true;
    ELSIF b > c THEN
        RETURN NULL;
    ELSE
        RETURN false;
    END IF;
END;
$$ LANGUAGE plpgsql;
SELECT range_greater_than('[6,9]'::int4range, '[2,5]'::int4range); -- true
SELECT range_greater_than('[3,6]'::int4range, '[4,8]'::int4range); -- null
SELECT range_greater_than('[1,3]'::int4range, '[4,7]'::int4range); -- false