
audb_test=# select c_range_set_add(array[int4range(1,3)], array[int4range(1,3), null]);
 c_range_set_add
-----------------
 {NULL,NULL}
(1 row)

Should not be this