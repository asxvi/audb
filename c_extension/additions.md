## 
* add aggregation logic - similar to prune functions, https://www.postgresql.org/docs/current/xaggr.html
* create test cases and answers in a runnable sql script
- [x] detect and handle lifting a i4r to an i4r set. overload funcitons pg_c functions to handle case 

additional optimizing
* reduce and convert boilerplate to macros 
* shell script entire process of building, running and testing


aggregate sum size therashold. extra parameter
if num of ranges is greater than extra param, then reduce and continue
or add 2 params, 1 target to start reducing 2




1/1/26 fixes
- [] go over arithmetic and fix empty results in Int4RangeSet   func removeEmpty()
- [] memory allocation. pass by ref vs val in self functions
- [] change rangeTypeOID and typCache to take many types other than I4R
- [] values that cross zero means ...?


1/10/26 Code Review
- [] logicalOperators.c && logicalOperators.h (any advice for .h files)
- [] arithmetic.c (confusion on division)
- []
- []