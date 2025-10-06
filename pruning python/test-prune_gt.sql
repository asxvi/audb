create or replace function prune_gt(set1 int4range[], set2 int4range[], direction bool)
RETURNS int4range[] AS $$
DECLARE
  rv int4range[] := '{}';
  curr1 int4range;
  curr2 int4range;
  candidate int4range;
BEGIN
 	if direction = false then
		FOREACH curr1 IN ARRAY normalize_vals(set1) LOOP
			FOREACH curr2 IN ARRAY normalize_vals(set2) LOOP
				IF curr1 >> curr2 THEN
        			rv := rv || (curr1);
				
				ELSIF curr1 && curr2 THEN
          			IF GREATEST(upper(curr2), lower(curr1)) < upper(curr1) THEN 
						candidate := int4range(GREATEST(upper(curr2), lower(curr1)), upper(curr1));
						IF NOT isempty(candidate) THEN
	          				rv := rv || candidate;
						END IF;
					END IF;
				END IF;
			END LOOP;
		END LOOP;
	else
		FOREACH curr1 IN ARRAY normalize_vals(set2) LOOP
			FOREACH curr2 IN ARRAY normalize_vals(set1) LOOP
				IF curr1 >> curr2 THEN
        			rv := rv || (curr1);
				
				ELSIF curr1 && curr2 THEN
					IF GREATEST(upper(curr2), lower(curr1)) < upper(curr1) THEN 
						candidate := int4range(GREATEST(upper(curr2), lower(curr1)), upper(curr1));
						IF NOT isempty(candidate) THEN
	          				rv := rv || candidate;
						END IF;
					END IF;
				END IF;
			END LOOP;
		END LOOP;
	end if;
  RETURN rv;
END;
$$ LANGUAGE plpgsql;