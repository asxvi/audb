CREATE OR REPLACE FUNCTION logical_3vl_and(a BOOLEAN, b BOOLEAN)
RETURNS BOOLEAN as $$
BEGIN
    IF a IS FALSE OR b IS FALSE THEN
        RETURN FALSE;
    ELSIF a IS TRUE AND b IS TRUE THEN
        RETURN TRUE;
    ELSE
        RETURN NULL;
    END IF;
END;
$$ LANGUAGE plpgsql;



select logical_3vl_and(true, NULL);
select logical_3vl_and(TRUE, FALSE);
select logical_3vl_and(TRUE, TRUE);



-- still not sure what these operators are for
CREATE OR REPLACE FUNCTION logical_3vl_or(a BOOLEAN, b BOOLEAN)
RETURNS BOOLEAN as $$
BEGIN
    IF a IS TRUE OR b IS TRUE THEN
        RETURN TRUE;
    ELSIF a IS FALSE AND b IS FALSE THEN
        RETURN FALSE;
    ELSE
        RETURN NULL;    
    END IF;
END;
$$ LANGUAGE plpgsql;

select logical_3vl_or(true, NULL);
select logical_3vl_or(TRUE, FALSE);
select logical_3vl_or(NULL, FALSE);
select logical_3vl_or(FALSE, FALSE);
select logical_3vl_or(TRUE, TRUE);





-- still not sure what these operators are for
CREATE OR REPLACE FUNCTION logical_3vl_not(a BOOLEAN)
RETURNS BOOLEAN as $$
BEGIN
    IF a IS TRUE THEN
        RETURN FALSE;
    ELSIF a IS FALSE THEN
        RETURN TRUE;
    ELSE
        RETURN NULL;
    END IF;
END;
$$ LANGUAGE plpgsql;

select logical_3vl_not(true);
select logical_3vl_not(FALSE);
select logical_3vl_not(NULL);



