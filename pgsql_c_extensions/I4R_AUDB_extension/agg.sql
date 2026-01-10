create aggregate sum (int4range[])
(
    stype = int4range[],
    -- initcond = array[int4range(0,0)],
    sfunc = c_range_set_add
);

create aggregate sum (int4range)
(
    sfunc = c_range_add,
    stype = int4range
    -- not sure why getting an error no matter what i use for initcond. tried int4range(0,1) and every other intuitive option
    -- initcond = '[0,0]'::int4range 
);

create aggregate max (int4range[])
(
    stype = int4range[],
    -- initcond = array[int4range(0,0)],
    sfunc = c_range_set_add
);

create aggregate max (int4range)
(
    stype = int4range,
    -- initcond = array[int4range(0,0)],
    sfunc = c_range_set_add
);


create aggregate min (int4range[])
(
    stype = int4range[],
    -- initcond = array[int4range(0,0)],
    sfunc = c_range_set_add
);

create aggregate min (int4range)
(
    stype = int4range,
    -- initcond = array[int4range(0,0)],
    sfunc = c_range_set_add
    -- should be min
);

create aggregate count()
(
    stype = int,
    initcond = 0::int4
    sfunc = c_range_set_add
    -- if equals then increment by 1. rv either 0 or 1
)


-- MAX and MIN
-- naive approach without optimizing
-- min_function(A int4range[], B int4range[])
for every i4r in A
    for every i4r in B
        min(a, b) -> i4r == (min(LB(a), LB(B)),  min(UB(a), UB(B)))

sum_function (existing_ranges, new_ranges, max_wanted_ranges, num_ranges_to_keep)
    add all ranges n x m
    if num_ranges in agg result >= max_wanted_ranges:
        reduce to num_ranges_to_keep


-- COUNT
for every row:
    compare if range is equal:
        if yes:
            count +=1
        else
            count +=0

-- AVG
for every

A                |              B
[(1,3), (5,7)]          [(1,6), (9,10)]
[(2,5), (6,7)]          [(1,6), (9,10)]

count(A) = 2
sum(A) = [(3,6), (7,8), (7,10), (11,12)]
avg(A) = [(1,2), (3,3), (3,4), (5,5)]


sum(A) after normalize = [(3,6), (7,10), (11,12)]
avg(A) after normalize = [(1,2), (3,4), (5,5)]
