-- using int4range instead of int[][], since int4range is more suitable for range comparison.
-- have different functions for each operation


CREATE OR REPLACE FUNCTION range_set_smallerthan(set1 int4range[], set2 int4range[]) RETURNS boolean AS $$
DECLARE
    lowest_a int;
    greatest_b int;
BEGIN
    lowest_a := lower(set1[1]);
    greatest_b := upper(set2[1]);
    IF lowest_a < greatest_b THEN
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION range_set_largerthan(set1 int4range[], set2 int4range[]) RETURNS boolean AS $$
DECLARE
    lowest_a int;
    greatest_b int;
BEGIN
    lowest_a := lower(set1[1]);
    greatest_b := upper(set2[1]);
    IF lowest_a > greatest_b THEN
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION range_set_equal(set1 int4range[], set2 int4range[]) RETURNS int4range[] AS $$
DECLARE
    pointer1 int;
    pointer2 int;
    result int4range[] := '{}';
    a int4range[];
    b int4range[];
BEGIN
    a := range_normalize(set1);
    b := range_normalize(set2);

    WHILE pointer1 <= array_length(a, 1) AND pointer2 <= array_length(b, 1) LOOP
        IF a[pointer1] = b[pointer2] THEN
            result := array_append(result, a[pointer1]);
            pointer1 := pointer1 + 1;
            pointer2 := pointer2 + 1;
        ELSIF a[pointer1] < b[pointer2] THEN
            pointer1 := pointer1 + 1;
        ELSE
            pointer2 := pointer2 + 1;
        END IF;
    END LOOP;

    RETURN result; 
END;
$$ LANGUAGE plpgsql;

-- CREATE OR REPLACE FUNCTION range_set_compare(set1 int4range[], set2 int4range[],
--                                              operator text) RETURNS boolean[] AS $$ DECLARE result boolean[]; BEGIN result := ARRAY[]::boolean[];
-- FOR i IN 1..array_length(set1, 1) LOOP IF
-- operator = '<' THEN result := array_append(result, set1[i] < set2[i]); -- don't sort here, just use for loop
--  -- exactly the same with larger, just switch places
--  -- a < b if there is a lowest value in a thats smaller than the greatest value of b
--  -- the opposite is much clearer: if the smallest value of a is NOT SMALLER than the greatst value of B, then that is false.
--  ELSIF
-- operator = '>' THEN result := array_append(result, set1[i] > set2[i]); ELSIF
-- operator = '=' THEN -- lsort and normalize all arrays in both sets
--  -- need to lsort on lower bound, so that instead of doing cross, you can just compare once per element order.
--  -- if no overlap, advance pointer to the next element.
--  -- advance lowest bound pointer so that you can check if there's either one
--  -- sort merge join algorithm
--  result := array_append(result, set1[i] = set2[i]); ELSE RAISE
-- EXCEPTION 'Invalid operator. Use <, >, or ='; END IF; END LOOP; RETURN result; END; $$ LANGUAGE plpgsql; -- not safe to do. assumes that theyre the same bound when there could be other possibilities
-- for equality, you need to test for overlap if any of the two intervals, that could also be true.


-- do all pairwise comparisons of intervals
 -- if you sort by lower bound you can use o(n log n)
-- same thing with smaller and larger than
 -- Example usage of the compare_range_sets function
-- SELECT compare_range_sets(
--      ARRAY['[100,200]'::int4range, '[1,3]'::int4range, '[500,700]'::int4range],
--      ARRAY['[900,1000]'::int4range, '[2000,5000]'::int4range, '[2,2]'::int4range],
--      '<'
-- );
 -- SELECT compare_range_sets(
--     ARRAY['[100,200]'::int4range, '[1,3]'::int4range, '[500,700]'::int4range],
--     ARRAY['[900,1000]'::int4range, '[2000,5000]'::int4range, '[2,2]'::int4range],
--     '>'
-- );
 -- SELECT compare_range_sets(
--     ARRAY['[100,200]'::int4range, '[1,3]'::int4range, '[500,700]'::int4range],