create table temp (
	A int4range[],
	B int4range[]
); 
insert into temp (A, B) values (array[int4range(1,9)], array[int4range(2,8)]), (array[int4range(0,2)], array[int4range(4,5)]);

select prune_gt(t.resA, 2), t.resb as b
from (
	select prune_lt(a, 3) as resA, b as resB
	from temp
	where range_less_than(a, array[int4range(3,4)]) is not false
) t





---with mult
create table tempMult (
	A int4range[],
	B int4range[],
	mult int4range
); 
insert into tempMult (A, B, mult) values (array[int4range(1,9)], array[int4range(2,8)], int4range(0,1)), (array[int4range(0,2)], array[int4range(4,5)], int4range(1,3));
