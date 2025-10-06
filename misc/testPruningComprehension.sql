CREATE OR REPLACE FUNCTION range_set_equal(set1 int4range[], set2 int4range[])
RETURNS BOOLEAN AS $$   
DECLARE 
    norm1 int4range[] := '{}';
    norm2 int4range[] := '{}';
    numPossible int := 0;
BEGIN
    norm1 := normalize_vals(set1);
    norm2 := normalize_vals(set2);

    IF array_length(norm1,1) IS NULL AND array_length(norm2,1) IS NULL THEN
        RETURN TRUE;
    END IF;
    IF array_length(norm1,1) IS NULL OR array_length(norm2,1) IS NULL THEN
        RETURN FALSE;
    END IF;

	-- vacuously true if a=b=c=d
    IF array_length(norm1,1) = 1 AND array_length(norm1,1) = array_length(norm2,1) THEN
        if (lower(norm1[1]) = upper(norm1[1])-1
			AND upper(norm1[1])-1 = lower(norm2[1])
			AND lower(norm2[1]) = upper(norm2[1])-1) THEN
			RETURN TRUE;
		END IF;
    END IF;
    

    -- -- only after normalizing the lengths shouldnt differ
    -- IF array_length(norm1, 1) != array_length(norm2, 1) THEN
    --     RETURN FALSE;
    -- END IF;
	
    -- go thru every range, and check if they can poissibly overlap == NULL rv
    FOR i in 1..array_length(norm1, 1) LOOP
        IF (norm1[i] = norm2[i]) OR (norm1[i] && norm2[i]) THEN
            --numPossible := numPossible + 1;
			RETURN NULL;
        END IF;
    END LOOP;

    IF numPossible = array_length(norm2, 1) THEN
        RETURN NULL;
    END IF;

    RETURN FALSE;
END;
$$ LANGUAGE plpgsql;

create table suExample(
	a int4range[],
	b int4range[],
	mult int4range
);


insert into suExample(a,b,mult) values (
	array[int4range(1,2),int4range(10,12)], 
	array[int4range(8,14)], 
	int4range(1,2)
)

------------------------------
--BASE QUERY
select a + 3 
from suExample
where a = b;
------------------------------

select range_set_add(a, array[int4range(3,4)]), int4range(lower(mult) * case when range_set_equal(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from suExample
where range_set_equal(a, b) is not False;


select range_set_add(prune_a, array[int4range(3,4)]), int4range(lower(mult) * case when range_set_equal(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from (
	select prune_eq(a,b,false) as prune_a, a, b, mult
	from suExample
) sub
where range_set_equal(a, b) is not False;


select range_set_add(prune_eq(a,b,false), array[int4range(3,4)]), b, int4range(lower(mult) * case when range_set_equal(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from suExample
where range_set_equal(a,b) is not false;

------------------------------
--BASE QUERY
select a < 11 
from suExample
where a > b;
------------------------------
 
select * from suexample;

select range_greater_than(a,b), int4range(lower(mult) * case when range_greater_than(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from suExample
where range_greater_than(a,b) is not false;


select range_less_than(prune_a, array[int4range(13,14)]), int4range(lower(mult) * case when range_greater_than(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from (
	select prune_lt(a,b,false) as prune_a, a, b, mult
	from suExample
)sub
where range_greater_than(a,b) is not false;



select prune_lt(prune_a, array[int4range(21,22)], false), int4range(lower(mult) * case when range_greater_than(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from (
	select prune_gt(a,b,false) as prune_a, a, b, mult
	from suExample
)sub
where range_greater_than(a,b) is not false;



--select prune_lt () prune_lt(a,array[int4range(11,12)],false) as prune_a, a, b, mult
--from suExample
--where range_greater_than(a,b) is not false;
