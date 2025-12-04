# 11/3/25

### extend extensions for variable leng types 
##### to see how/what might be needed when making template function
* select c_range_set_add(array[int4range(1,6), int4range(5,9)], array[int4range(1,3), int4range(5,10)]);
var length; extend for range of date or ts
* mutilply float range by int range. 



### optimize running and testing script while devloping
* consider using a sql script and passing it to psql
    * psql -f[ilename] flag (pseudo example)
* consider shell scripting





## 
* add aggregation logic - similar to prune functions, https://www.postgresql.org/docs/current/xaggr.html
* create test cases and answers in a runnable sql script

additional optimizing
* reduce and convert boilerplate to macros 
* shell script entire process of building, running and testing
