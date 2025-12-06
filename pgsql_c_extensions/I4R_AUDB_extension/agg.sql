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
