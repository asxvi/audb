DROP TABLE range_arithmetic_tests;
CREATE TEMP TABLE range_arithmetic_tests (
    name text,
    actual int4range,
    expected int4range
);
INSERT INTO range_arithmetic_tests VALUES
-- 1 input arithmetic
('c_range_add_1_input',
 c_range_add('[1,6)', '[9,21)'),
 '[10,26)'),

('c_range_sub_1_input',
 c_range_subtract('[1,6)', '[9,21)'),
 '[-19,-3)'),

('c_range_mult_1_input',
 c_range_multiply('[1,6)', '[9,21)'),
 '[9, 101)'),

('c_range_div_1_input',
 c_range_divide('[3,10)', '[3,4)'),
 '[1, 4)'),

('c_range_add_empty_P1',
 c_range_add('empty'::int4range, '[1,5)'),
 '[1, 5)'),

('c_range_add_empty_P2',
 c_range_add('[1,5)', 'empty'::int4range),
 '[1, 5)'),

('c_range_add_empty',
 c_range_add('empty'::int4range, 'empty'::int4range),
 'empty'::int4range),

('c_range_sub_empty_P1',
 c_range_subtract('empty'::int4range, '[1,5)'),
 'empty'::int4range),

('c_range_sub_empty_P2',
 c_range_subtract('[1,5)', 'empty'::int4range),
 '[1, 5)'),

('c_range_sub_empty',
 c_range_subtract('empty'::int4range, 'empty'::int4range),
 'empty'::int4range),

('c_range_mult_empty_P1',
 c_range_multiply('empty'::int4range, '[1,5)'),
 'empty'::int4range),

('c_range_mult_empty_P2',
 c_range_multiply('[1,5)', 'empty'::int4range),
 'empty'::int4range),

('c_range_mult_empty',
 c_range_multiply('empty'::int4range, 'empty'::int4range),
 'empty'::int4range),

('c_range_div_empty_P1',
 c_range_divide('empty'::int4range, '[1,5)'),
 'empty'::int4range),

('c_range_div_empty_P2',
 c_range_divide('[1,5)', 'empty'::int4range),
 'empty'::int4range),

('c_range_div_empty',
 c_range_divide('empty'::int4range, 'empty'::int4range),
 'empty'::int4range);


DROP TABLE set_arithmetic_tests;
CREATE TEMP TABLE set_arithmetic_tests (
    name text,
    actual int4range[],
    expected int4range[]
);

INSERT INTO set_arithmetic_tests VALUES
('c_set_add',
 c_range_set_add(array[int4range(1,3), int4range(5,9)], array[int4range(1,3), int4range(5,9)]),
 array[int4range(2,5), int4range(6,11), int4range(6,11), int4range(10, 17)]);
-- ('c_set_add',
--  c_range_set_add(array[int4range(1,3), int4range(5,9)], array[int4range(1,3), int4range(5,9)]),
--  array[int4range(2,5), int4range(6,11), int4range(6,11), int4range(10, 17)]),


DROP TABLE IF EXISTS range_agg_data_pos;
DROP TABLE IF EXISTS range_agg_data_neg;
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
FROM range_agg_data_neg;


-- show failures
SELECT *, 'range_arithmetic_tests' as source
FROM range_arithmetic_tests
WHERE actual IS DISTINCT FROM expected;

SELECT *, 'set_arithmetic_tests' as source
FROM set_arithmetic_tests
WHERE actual IS DISTINCT FROM expected;

SELECT *, 'range_agg_tests' as source
FROM range_agg_tests
WHERE actual IS DISTINCT FROM expected;