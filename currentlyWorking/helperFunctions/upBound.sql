CREATE OR REPLACE FUNCTION upBound(x integer)
RETURNS int4range as $$
BEGIN
    RETURN int4range(x,x,'[]');
END
$$ LANGUAGE plpgsql;