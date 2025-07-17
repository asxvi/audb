-- i think this works
CREATE OR REPLACE FUNCTION range_set_multiply(set1 int4range[], set2 int4range[])
RETURNS int4range[] AS $$
DECLARE
    result int4range[] := '{}';
    i int4range; j int4range;
 	p1 int; p2 int; p3 int; p4 int;
BEGIN
    if array_length(set1, 1) is NULL then
        RETURN set2;
    end if;

    if array_length(set2, 1) is NULL THEN
        RETURN set1;
    end if;

    FOR i IN (SELECT unnest(set1)) LOOP
        FOR j IN (SELECT unnest(set2)) LOOP
			p1 = lower(i) * lower(j);
            p2 = lower(i) * upper(j);
            p3 = upper(i) * lower(j);
            p4 = upper(i) * upper(j);

            -- result := array_append(result, int4range((lower(i) * lower(j)), (upper(i) * upper(j)) + 1));
            result := array_append(result, int4range(LEAST(p1, p2, p3, p4), GREATEST(p1, p2, p3, p4) + 1));
        END LOOP;
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;