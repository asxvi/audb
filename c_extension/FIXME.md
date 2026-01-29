
audb_test=# select c_range_set_add(array[int4range(1,3)], array[int4range(1,3), null]);
 c_range_set_add
-----------------
 {NULL,NULL}
(1 row)

Should not be this




NULL vs Empty arithmetic:

audb_test=# select c_range_set_add(array[]::int4range[], array[int4range(1,5)]);
 c_range_set_add
-----------------
 {}
(1 row)

audb_test=# select c_range_set_add(null, array[int4range(1,5)]);
 c_range_set_add
-----------------
 {"[1,5)"}
(1 row)