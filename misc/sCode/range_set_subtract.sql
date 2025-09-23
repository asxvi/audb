 CREATE OR REPLACE FUNCTION range_set_subtract(l int[][], r int[][])
 RETURNS int[][] AS $$
 DECLARE
    result int[][];
    i int;
    j int;
BEGIN
    -- Initialize result array
    result := '{}';

    -- double for loop to iterate throuh all elements of l and r
    FOR i IN array_lower(l, 1)..array_upper(l, 1) LOOP
        FOR j IN array_lower(r, 1)..array_upper(r, 1) LOOP
        
            result := result || ARRAY[ARRAY[
                    l[i][1] - r[j][2],
                    l[i][2] - r[j][1]
                ]];
        END LOOP;
    END LOOP;

    RETURN result;
END;
$$ LANGUAGE plpgsql;

