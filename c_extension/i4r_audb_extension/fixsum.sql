drop extension i4r_audb_extension;

create extension i4r_audb_extension;

\i i4r_audb_extension_test--1.1.sql

SELECT id, cola, mult,
       combine_set_mult_sum(cola, mult) as result,
       combine_set_mult_sum(cola, mult) IS NULL as is_null
FROM t6_s_agg_data
ORDER BY id;

select sum2(combine_set_mult_sum(cola, mult), 10, 10) from t6_s_agg_data;   