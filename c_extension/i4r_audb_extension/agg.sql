-- Count is implemented natively
-- Sum works but explodes m^n where m is the avg num of elements in each set, n is number of rows. figure out optimizations
-- Min naive approach can be optimized
-- Max naive approach can be optimized
-- Avg sum/count

-- test sum agg
create aggregate sum (int4range[], int4range)
(
    stype = internal,       -- Type: IntervalAggState
    sfunc = agg_sum_interval_transfunc,
    finalfunc = agg_sum_interval_finalfunc
);

-- -- test min agg
-- create aggregate min_range_test (internal)
-- (
--     stype = internal,       -- Type: RangeRowType
--     sfunc = agg_min_transfunc,
--     finalfunc = agg_min_finalfunc
-- );

create aggregate min (int4range)
(
    stype = int4range,
    sfunc = agg_min_transfunc
    -- finalfunc -- dont need
);
create aggregate max (int4range)
(
    stype = int4range,
    sfunc = agg_max_transfunc
    -- finalfunc -- dont need
);

-- -- deal with blowup with hardcoded value
-- -- initcond = NULL by default
-- create aggregate sum (int4range[])
-- (
--     stype = int4range[],
--     sfunc = c_range_set_add
-- );

-- -- initcond = NULL by default
-- create aggregate sum (int4range)
-- (
--     stype = int4range,
--     sfunc = c_range_add
-- );

-- create aggregate max (int4range[])
-- (
--     stype = int4range[],
--     -- initcond = array[int4range(0,0)],
--     sfunc = 
-- );

-- create aggregate max (int4range)
-- (
--     stype = int4range,
--     -- initcond = array[int4range(0,0)],
--     sfunc = 
-- );


-- create aggregate min (int4range[])
-- (
--     stype = int4range[],
--     -- initcond = array[int4range(0,0)],
--     sfunc = 
-- );

-- create aggregate min (int4range)
-- (
--     stype = int4range,
--     initcond = int4range(0,0),
--     sfunc = 
-- );

-- create aggregate avg (int4range)
-- (
--     stype = int4range,
--     -- stype (sum [], count int)
--     -- finalfunc
-- );

-- -- MAX and MIN
-- -- naive approach without optimizing
-- -- min_function(A int4range[], B int4range[])
-- for every i4r in A
--     for every i4r in B
--         min(a, b) -> i4r == (min(LB(a), LB(B)),  min(UB(a), UB(B)))

-- sum_function (existing_ranges, new_ranges, max_wanted_ranges, num_ranges_to_keep)
--     add all ranges n x m
--     if num_ranges in agg result >= max_wanted_ranges:
--         reduce to num_ranges_to_keep

-- -- AVG
-- for every

-- A                |              B
-- [(1,3), (5,7)]          [(1,6), (9,10)]
-- [(2,5), (6,7)]          [(1,6), (9,10)]

-- count(A) = 2
-- sum(A) = [(3,6), (7,8), (7,10), (11,12)]
-- avg(A) = [(1,2), (3,3), (3,4), (5,5)]


-- sum(A) after normalize = [(3,6), (7,10), (11,12)]
-- avg(A) after normalize = [(1,2), (3,4), (5,5)]


