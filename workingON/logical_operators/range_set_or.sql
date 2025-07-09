-- need to handle null values or invalid/ empty set
CREATE OR REPLACE FUNCTION range_set_or(set1 int4range, set2 int4range) RETURNS int4range AS $$
BEGIN
    RETURN int4range[
        LEAST(lower(set1), lower(set2)), GREATEST(upper(set1),upper(set2))
    ];
END;
$$ LANGUAGE plpgsql;

