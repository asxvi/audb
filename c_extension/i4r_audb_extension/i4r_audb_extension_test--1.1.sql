

-- ===================================
-- Test1: Range Arithmetic
-- ===================================
DROP TABLE t1_r_arithmetic;
CREATE TEMP TABLE t1_r_arithmetic (
    name text,
    actual int4range,
    expected int4range
);
INSERT INTO t1_r_arithmetic VALUES
    -- basic arithmetic
    ('add_basic', range_add('[1,6)', '[9,21)'), '[10,26)'),
    ('subtract_basic', range_subtract('[1,6)', '[9,21)'), '[-19,-3)'),
    ('multiply_basic', range_multiply('[1,6)', '[9,21)'), '[9,101)'),
    ('divide_basic', range_divide('[3,10)', '[3,4)'), '[1,4)'),
    
    -- empty cases- add
    ('add_empty_param1', range_add('empty'::int4range, '[1,5)'), '[1,5)'),
    ('add_empty_param2', range_add('[1,5)', 'empty'::int4range), '[1,5)'),
    ('add_both_empty', range_add('empty'::int4range, 'empty'::int4range), 'empty'::int4range),
    
    -- empty cases- sub
    ('subtract_empty_param1', range_subtract('empty'::int4range, '[1,5)'), 'empty'::int4range),
    ('subtract_empty_param2', range_subtract('[1,5)', 'empty'::int4range), '[1,5)'),
    ('subtract_both_empty', range_subtract('empty'::int4range, 'empty'::int4range), 'empty'::int4range),
    
    -- empty cases- mult
    ('multiply_empty_param1', range_multiply('empty'::int4range, '[1,5)'), 'empty'::int4range),
    ('multiply_empty_param2', range_multiply('[1,5)', 'empty'::int4range), 'empty'::int4range),
    ('multiply_both_empty', range_multiply('empty'::int4range, 'empty'::int4range), 'empty'::int4range),
    
    -- empty cases- div
    ('divide_empty_param1', range_divide('empty'::int4range, '[1,5)'), 'empty'::int4range),
    ('divide_empty_param2', range_divide('[1,5)', 'empty'::int4range), 'empty'::int4range),
    ('divide_both_empty', range_divide('empty'::int4range, 'empty'::int4range), 'empty'::int4range);


-- ===================================
-- Test2: Set Arithmetic
-- ===================================
DROP TABLE t2_s_arithmetic;
CREATE TEMP TABLE t2_s_arithmetic (
    name text,
    actual int4range[],
    expected int4range[]
);

INSERT INTO t2_s_arithmetic VALUES
    -- basic arithmetic
    ('add_basic', set_add(array[int4range(1,3), int4range(2,4)], array[int4range(1,3), int4range(2,4)]), array[int4range(2,5), int4range(3,6), int4range(3,6), int4range(4,7)]),
    ('subtract_basic', set_subtract(array[int4range(10,21), int4range(25,51)], array[int4range(5,11), int4range(10, 16)]), array[int4range(0,15), int4range(-5,10), int4range(15,45), int4range(10,40)]),
    ('multiply_basic', set_multiply(array[int4range(1,3), int4range(2,4)], array[int4range(1,3), int4range(2,4)]), array[int4range(1,5), int4range(2,7), int4range(2,7), int4range(4,10)]),
    ('divide_basic', set_divide(array[int4range(10,13), int4range(20,41)], array[int4range(2,3), int4range(4,5)]), array[int4range(5,7), int4range(2,4), int4range(10,21), int4range(5,11)]),

    -- empty cases- add
    ('set_add_empty_param1', set_add(array[]::int4range[], array[int4range(1,5)]), array[int4range(1,5)]),
    ('set_add_empty_param2', set_add(array[int4range(1,5)], array[]::int4range[]), array[int4range(1,5)]),
    ('set_add_both_empty', set_add(array[]::int4range[], array[]::int4range[]), array[]::int4range[]),

    -- empty cases- sub
    ('set_subtract_empty_param1', set_subtract(array[]::int4range[], array[int4range(1,5)]), array[]::int4range[]),
    ('set_subtract_empty_param2', set_subtract(array[int4range(1,5)], array[]::int4range[]), array[int4range(1,5)]),
    ('set_subtract_both_empty', set_subtract(array[]::int4range[], array[]::int4range[]), array[]::int4range[]),

    -- empty cases- mult
    ('set_multiply_empty_param1', set_multiply(array[]::int4range[], array[int4range(1,5)]), array[]::int4range[]),
    ('set_multiply_empty_param2', set_multiply(array[int4range(1,5)], array[]::int4range[]), array[]::int4range[]),
    ('set_multiply_both_empty', set_multiply(array[]::int4range[], array[]::int4range[]), array[]::int4range[]),

    -- empty cases- div
    ('set_divide_empty_param1', set_divide(array[]::int4range[], array[int4range(1,5)]), array[]::int4range[]),
    ('set_divide_empty_param2', set_divide(array[int4range(1,5)], array[]::int4range[]), array[]::int4range[]),
    ('set_divide_both_empty', set_divide(array[]::int4range[], array[]::int4range[]), array[]::int4range[]);


-- ===================================
-- Test3: Logical Operators
-- ===================================

DROP TABLE IF EXISTS t3_r_logical;
CREATE TEMP TABLE t3_r_logical (
    name text,
    actual int4range,
    expected int4range
);

INSERT INTO t3_r_logical VALUES
    ('lt_basic', set_lt(int4range(1,3), int4range(20,40)), true),
    



-- convert to colA pos, colB neg
DROP TABLE IF EXISTS range_agg_data_pos;
DROP TABLE IF EXISTS range_agg_data_neg;
DROP TABLE IF EXISTS range_sum_data;
CREATE TEMP TABLE IF NOT EXISTS range_agg_data_pos (
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range,
    colB int4range,
    mult int4range
);

CREATE TEMP TABLE IF NOT EXISTS range_agg_data_neg (
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range,
    colB int4range,
    mult int4range
);

CREATE TEMP TABLE IF NOT EXISTS range_sum_data (
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range,
    mult int4range
);


INSERT INTO range_agg_data_pos (colA, colB, mult) VALUES
    (int4range(1,1000), int4range(200,400), int4range(0,2)),
    (int4range(9,11), int4range(4,9), int4range(1,2)),
    (int4range(10,13), int4range(1,12), int4range(1,7)),
    (int4range(100,130), int4range(12,1400), int4range(6,7)),
    (int4range(6,22), int4range(121,122), int4range(2,4)),
    (int4range(44,332), int4range(12,14), int4range(5,6)),
    ('empty'::int4range, 'empty'::int4range, 'empty'::int4range),
    (int4range(1, 2), int4range(1, 2), 'empty'::int4range),
    ('empty'::int4range, int4range(23,34), int4range(5,6)),
    (int4range(24,34), 'empty'::int4range, int4range(5,6)),
    (NULL, NULL, NULL),
    (int4range(1, 2), int4range(100,102), NULL),
    (int4range(NULL), int4range(100,102), NULL);

INSERT INTO range_agg_data_neg (colA, colB, mult) VALUES
    (int4range(-1000,-1), int4range(-400,-200), int4range(0,2)),
    (int4range(-11,-9), int4range(-9,-4), int4range(1,2)),
    (int4range(-13,-10), int4range(-12,-1), int4range(1,7)),
    (int4range(-130,-100), int4range(-1400,-12), int4range(6,7)),
    (int4range(-22,-6), int4range(-122,-121), int4range(2,4)),
    (int4range(-332,-44), int4range(-14,-12), int4range(5,6)),
    ('empty'::int4range, 'empty'::int4range, 'empty'::int4range),
    (int4range(-2, -1), int4range(-2, -1), 'empty'::int4range),
    ('empty'::int4range, int4range(-34,-23), int4range(5,6)),
    (int4range(-34,-23), 'empty'::int4range, int4range(5,6)),
    (NULL, NULL, NULL),
    (int4range(-2, -1), int4range(-102,-100), NULL),
    (int4range(NULL), int4range(-102,-100), NULL);

INSERT INTO range_sum_data (colA, mult) VALUES
    (int4range(1,3), int4range(1,2)),
    (int4range(2,4), int4range(1,2)),
    (int4range(3,5), int4range(1,2)),
    (int4range(4,6), int4range(1,2)),
    (int4range(5,7), int4range(1,2)),
    (int4range(1000,2000), int4range(0,2)),
    (int4range(2000,3000), NULL),
    (NULL, int4range(1,2)),
    (NULL, NULL);

DROP TABLE IF EXISTS set_min_max_test;
CREATE TEMP TABLE IF NOT EXISTS set_min_max_test (
    id int GENERATED ALWAYS AS IDENTITY,
    colA int4range[],
    colB int4range[],
    mult int4range
);

INSERT INTO set_min_max_test (colA, colB, mult) VALUES
    ( array[int4range(1,3), int4range(6,10), int4range(20,30)], array[int4range(1,1000), int4range(2000,3000), int4range(4000,5000)], int4range(1,4) ),
    ( array[int4range(2,3), int4range(9,14)], array[int4range(43,99), int4range(2500,3100), int4range(2250,2790)], int4range(1,4));

DROP TABLE IF EXISTS range_agg_tests;
CREATE TEMP TABLE range_agg_tests (
    name text,
    actual int4range,
    expected int4range
);

INSERT INTO range_agg_tests (name, actual, expected) 
    -- positives only
    SELECT
        'min_range_basic_cola',
        min(combine_range_mult_min(cola, mult)),
        int4range(6, 11)
    FROM range_agg_data_pos

    UNION ALL

    SELECT
        'min_range_basic_colb',
        min(combine_range_mult_min(colb, mult)),
        int4range(1, 9)
    FROM range_agg_data_pos

    UNION ALL

    SELECT
        'max_range_basic_cola',
        max(combine_range_mult_max(cola, mult)),
        int4range(100, 332)
    FROM range_agg_data_pos

    UNION ALL

    SELECT
        'max_range_basic_colb',
        max(combine_range_mult_max(colb, mult)),
        int4range(121, 1400)
    FROM range_agg_data_pos

    UNION ALL

    -- negatives only
    SELECT
        'min_range_basic_cola',
        min(combine_range_mult_min(cola, mult)),
        int4range(-332, -100)
    FROM range_agg_data_neg

    UNION ALL

    SELECT
        'min_range_basic_colb',
        min(combine_range_mult_min(colb, mult)),
        int4range(-1400, -121)
    FROM range_agg_data_neg

    UNION ALL

    SELECT
        'max_range_basic_cola',
        max(combine_range_mult_max(cola, mult)),
        int4range(-11, -6)
    FROM range_agg_data_neg

    UNION ALL

    SELECT
        'max_range_basic_colb',
        max(combine_range_mult_max(colb, mult)),
        int4range(-9, -1)
    FROM range_agg_data_neg

    UNION ALL

    SELECT
        'sum_range_basic',
        sum(combine_range_mult_sum(cola, mult)),
        int4range(15, 21)
    FROM range_sum_data
;

DROP TABLE IF EXISTS set_agg_tests;
CREATE TEMP TABLE set_agg_tests (
    name text,
    actual int4range[],
    expected int4range[]
);

INSERT INTO set_agg_tests (name, actual, expected) 
    SELECT
        'min_set_basic_cola',
        min(combine_set_mult_min(cola, mult)),
        array[int4range(1, 3), int4range(6,14)]
    FROM set_min_max_test

    UNION ALL

    SELECT
        'min_set_basic_colb',
        min(combine_set_mult_min(colb, mult)),
        array[int4range(1, 1000), int4range(2000,3100)]
    FROM set_min_max_test

    UNION ALL

    SELECT
        'max_set_basic_cola',
        max(combine_set_mult_max(cola, mult)),
        array[int4range(2, 3), int4range(6,14), int4range(20, 30)]
    FROM set_min_max_test

    UNION ALL

    SELECT
        'max_set_basic_colb',
        max(combine_set_mult_max(colb, mult)),
        array[int4range(43, 1000), int4range(2250,3100), int4range(4000, 5000)]
    FROM set_min_max_test
;


-- show failures
SELECT *, 't1_r_arithmetic' as source
FROM t1_r_arithmetic
WHERE actual IS DISTINCT FROM expected;

SELECT *, 't2_s_arithmetic' as source
FROM t2_s_arithmetic
WHERE actual IS DISTINCT FROM expected;

SELECT *, 'range_agg_tests' as source
FROM range_agg_tests
WHERE actual IS DISTINCT FROM expected;

SELECT *, 'set_agg_tests' as source
FROM set_agg_tests
WHERE actual IS DISTINCT FROM expected;




-- init test data

-- DROP TABLE IF EXISTS r1;
-- DROP TABLE IF EXISTS s1;

-- CREATE TABLE IF NOT EXISTS r1(
--     id int GENERATED ALWAYS AS IDENTITY,
--     colA int4range,
--     colB int4range,
--     mult int4range
-- );

-- INSERT INTO r1 (colA, colB, mult) VALUES
--     (int4range(1,1000), int4range(200,400), int4range(0,2)),
--     (int4range(9,11), int4range(4,9), int4range(1,2)),
--     (int4range(10,13), int4range(1,12), int4range(1,7)),
--     (int4range(100,130), int4range(12,1400), int4range(6,7)),
--     (int4range(6,7), int4range(121,122), int4range(2,4)),
--     (int4range(44,332), int4range(12,14), int4range(5,6)),
--     ('empty'::int4range, int4range(23,34), int4range(5,6)),
--     (int4range(24,34), 'empty'::int4range, int4range(5,6));


-- CREATE TABLE IF NOT EXISTS s1(
--     id int GENERATED ALWAYS AS IDENTITY,
--     colA int4range[],
--     colB int4range[],
--     mult int4range
-- );

-- INSERT INTO s1 (colA, colB, mult) VALUES
--     (array[int4range(1,5), int4range(9,12), int4range(18,29)], array[int4range(4,13), int4range(16,20)], int4range(1,1, '[]')),
--     (array[int4range(4,13), int4range(16,20)], array[int4range(22,25), int4range(34,50)], int4range(1,1, '[]')),
--     (array[int4range(1,5), int4range(9,12), int4range(18,29)], array[int4range(4,13), int4range(16,20)], int4range(0,1, '[]'));
