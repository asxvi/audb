CREATE OR REPLACE FUNCTION range_set_equal(set1 int4range[], set2 int4range[])
RETURNS BOOLEAN AS $$
DECLARE
    a int := lower(set1[1]);
    b int := upper(set1[1]);
    c int := lower(set2[1]);
    d int := upper(set2[1]);
    RAISE NOTICE 'a is: %', a;
    RAISE NOTICE 'b is: %', b;
    RAISE NOTICE 'c is: %', c;
    RAISE NOTICE 'd is: %', d;
    global_min int; 
    global_max int;
    max_range int;
    total_range int;
BEGIN
    IF a = b AND b = c AND c = d THEN
        RETURN true;
    END IF;

    global_min := LEAST(a, c);
    global_max := GREATEST(b, d);
    max_range := global_max - global_min;
    total_range := (b - a) + (d - c);

    IF max_range > total_range THEN
        RETURN false; -- disjoint range not equal
    ELSE
        RETURN null; -- some overlap but not equal
    END IF;
END;
$$ LANGUAGE plpgsql;