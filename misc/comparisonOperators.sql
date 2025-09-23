-- assume that parameters are not normalized. If want to normalize, call normalize, and then just index at set[1], set[last]
CREATE OR REPLACE FUNCTION range_greater_than(set1 int4range[], set2 int4range[])
RETURNS boolean AS $$
DECLARE
    n1 int4range[];
    n2 int4range[];
    f1 int4range;
    f2 int4range;
    l1 int4range;
    l2 int4range;
BEGIN 
    -- check for empty sets 
    IF array_length(set1, 1) IS NULL OR array_length(set2, 1) IS NULL THEN
        RETURN NULL;
    END IF;

    n1 := normalize_range(set1);
    n2 := normalize_range(set2);

    f1 := n1[1];
    f2 := n2[1];
    l1 := n1[array_length(n1,1)];
    l2 := n2[array_length(n2,1)];   

    IF lower(f1) >= upper(l2) THEN
        RETURN TRUE;
    ELSIF upper(l1) <= lower(f2) THEN
        RETURN FALSE;
    ELSE
        RETURN NULL;
    END IF;
END;
$$ LANGUAGE plpgsql;

select range_greater_than(ARRAY[int4range(3,5)], array[int4range(4,6)]);
select int4range(3,5) > int4range(4,6);
select int4range(5,6) > int4range(3,8);
-- [(1,4), (7, 10)] > [(2, 5)]


-- assume that parameters are not normalized. If want to normalize, call normalize, and then just index at set[1], set[last]
CREATE OR REPLACE FUNCTION range_less_than(set1 int4range[], set2 int4range[])
RETURNS boolean AS $$
DECLARE
    n1 int4range[];
    n2 int4range[];
    f1 int4range;
    f2 int4range;
    l1 int4range;
    l2 int4range;
BEGIN 
    -- check for empty sets 
    IF array_length(set1, 1) IS NULL OR array_length(set2, 1) IS NULL THEN
        RETURN NULL;
    END IF;

    n1 := normalize_range(set1);
    n2 := normalize_range(set2);

    f1 := n1[1];
    f2 := n2[1];
    l1 := n1[array_length(n1,1)];
    l2 := n2[array_length(n2,1)];   

    IF lower(f2) >= upper(l1) THEN
        RETURN TRUE;
    ELSIF upper(l2) <= lower(f1) THEN
        RETURN FALSE;
    ELSE
        RETURN NULL;
    END IF;
END;
$$ LANGUAGE plpgsql;

select range_less_than(ARRAY[int4range(3,5)], array[int4range(1,2)]);



