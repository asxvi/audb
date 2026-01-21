DROP TABLE range_tests;

CREATE TEMP TABLE range_tests (
    name text,
    actual int4range,
    expected int4range
);

INSERT INTO range_tests VALUES
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


-- show failures
SELECT *
FROM range_tests
WHERE actual IS DISTINCT FROM expected;
