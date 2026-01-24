DROP TABLE IF EXISTS test_set;
DROP TABLE IF EXISTS test_range;
DROP TABLE IF EXISTS test_rset1;
DROP TABLE IF EXISTS test_rset2;
DROP TABLE IF EXISTS test_rset3;

CREATE TABLE IF NOT EXISTS test_rset1(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range[],
    colB int4range[],
    mult int4range
);
CREATE TABLE IF NOT EXISTS test_rset2(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range[],
    colB int4range[],
    mult int4range
);
CREATE TABLE IF NOT EXISTS test_rset3(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range[],
    colB int4range[],
    mult int4range
);
INSERT INTO test_rset1 (colA, colB, mult) VALUES
    (array[int4range(1,2), int4range(10,16)], array[int4range(2,3)], int4range(1,1, '[]')),
    (array[int4range(2,3), int4range(10,16)], array[int4range(10,12)], int4range(1,1, '[]')),
    (array[int4range(3,4), int4range(1,3)], array[int4range(8,15)], int4range(1,1, '[]'));


-- INSERT INTO test_rset2 (colA, colB) VALUES
--     (array[int4range(1,2), int4range(5,6)], array[int4range(2,3)]),
--     (array[int4range(2,3), int4range(10,16)], array[int4range(10,12)]),
--     (array[int4range(3,4)], array[int4range(8,15)]);

-- INSERT INTO test_rset3 (colA, colB) VALUES
--     (array[int4range(1,2), int4range(5,6), int4range(7,9)], array[int4range(2,3)]),
--     (array[int4range(2,3), int4range(10,16), int4range(5,6)], array[int4range(10,12)]),
--     (array[int4range(3,4), int4range(5,6), int4range(5,6)], array[int4range(8,15)]);


-- CREATE TABLE IF NOT EXISTS test_set(
--     id int GENERATED ALWAYS AS IDENTITY,
--     colA int4range[],
--     colB int4range[]
-- );
-- INSERT INTO test_set (colA, colB) VALUES
--     (array[int4range(1,2)], array[int4range(2,3)]),
--     (array[int4range(2,3)], array[int4range(10,12)]),
--     (array[int4range(3,4)], array[int4range(8,15)]);

CREATE TABLE IF NOT EXISTS test_range(
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range,
    colB int4range
);
INSERT INTO test_range (colA, colB) VALUES
    (int4range(1,3), int4range(2,4) ),
    (int4range(2,11), int4range(4,9) ),
    (int4range(10,13), int4range(12,14) );


