CREATE OR REPLACE FUNCTION prune_gt(set1 int4range[], set2 int4range[], direction bool)
RETURNS int4range[] AS $$
DECLARE
  rv int4range[] := '{}';
  curr int4range;
  norm1 int4range[];
  norm2 int4range[];
  minB int;
  maxA int;
  pruned int4range;
BEGIN
  norm1 := normalize_vals(set1);
  norm2 := normalize_vals(set2);

  -- A > B; A = A n [minB, inf]
  IF direction = FALSE THEN 
    -- not sure what to do on CASE when A < NULL
		IF array_length(norm2,1) is NULL THEN
      RETURN norm1;
    END IF;

    SELECT min(lower(ranges)) INTO minB
    FROM unnest(norm2) as ranges
    WHERE lower(ranges) is not NULL;

	  -- nothing to prune above
    IF minB IS NULL THEN
      RETURN rv;  
    END IF;

    FOREACH curr IN ARRAY norm1 LOOP
      pruned := curr * int4range(minB, NULL);
      IF NOT isempty(pruned) THEN
        rv := rv || pruned;  
      END IF;
    END LOOP;  
  -- A < B; B = B n [-inf, max(A)]
  ELSE
    -- not sure what to do on CASE when A < NULL
		IF array_length(norm2,1) is NULL THEN
      RETURN norm1;
    END IF;

    SELECT max(upper(ranges)) INTO maxA
    FROM unnest(norm1) as ranges
    WHERE upper(ranges) is not NULL;

    -- nothing to prune above
    IF maxA IS NULL THEN
      RETURN rv;  
    END IF;

    FOREACH curr IN ARRAY norm2 LOOP
      pruned := curr * int4range(NULL, maxA);
      IF NOT isempty(pruned) THEN
        rv := rv || pruned;  
      END IF;
    END LOOP;  
  END IF;
  RETURN rv;
END;
$$ LANGUAGE plpgsql;


select * from borisExample;
-- [[1,10)] > [[3,6)]
select prune_gt(A, B, false) as A, prune_gt(A, B, true) as B, 
int4range(lower(mult) * case when range_greater_than(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from borisExample;

--{"[3,10)"}|{"[3,6)"}|[0,2)|



select * from suExample;
--{"[1,2)","[10,12)"}|{"[8,14)"}|[1,2)|
--{"[1,2)","[10,30)"}|{"[8,14)"}|[0,1)|
--{"[1,10)"}         |{"[3,6)"} |[1,2)|

select prune_gt(A, B, false) as A, prune_gt(A, B, true) as B, 
int4range(lower(mult) * case when range_greater_than(a,b) is NULL then 0 else 1 end, upper(mult)) as mult
from suExample;
--{"[1,2)","[10,12)"}|{"[8,14)"}|[0,2)|
--{"[1,2)","[10,14)"}|{"[8,14)"}|[0,1)|
--{"[1,6)"}          |{"[3,6)"} |[0,2)|

--{"[10,12)"}|{"[8,12)"}|[0,2)|
--{"[10,30)"}|{"[8,14)"}|[0,1)|
--{"[3,10)"} |{"[3,6)"} |[0,2)|
