INSERT INTO t4_s_logical (name, code, actual, expected) VALUES
    -- true results
    ('lt_true', 'set_lt(array[int4range(1,3), int4range(5,7)], array[int4range(10,30), int4range(50,70)])', set_lt(array[int4range(1,3), int4range(5,7)], array[int4range(10,30), int4range(50,70)]), true),
    ('lte_true', 'set_lte(array[int4range(1,3), int4range(5,11)], array[int4range(10,30), int4range(50,70)])', set_lte(array[int4range(1,3), int4range(5,11)], array[int4range(10,30), int4range(50,70)]), true),
    ('gt_true', 'set_gt(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,7)])', set_gt(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,7)]), true),
    ('gte_true', 'set_gte(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,11)])', set_gte(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,11)]), true),
    -- false results
    ('lt_false', 'set_lt(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,7)])', set_lt(array[int4range(10,30), int4range(50,70)], array[int4range(1,3), int4range(5,7)]), false),
    ('lte_false', 'set_lte(array[int4range(21,40)], array[int4range(1,21)])', set_lte(array[int4range(21,40)], array[int4range(1,21)]), false),
    ('gt_false', 'set_gt(array[int4range(1,3), int4range(5,7)], array[int4range(10,30), int4range(50,70)])', set_gt(array[int4range(1,3), int4range(5,7)], array[int4range(10,30), int4range(50,70)]), false),
    ('gte_false', 'set_gte(array[int4range(1,3)], array[int4range(3,40)])', set_gte(array[int4range(1,3)], array[int4range(3,40)]), false),
    -- null results
    ('lt_overlap', 'set_lt(array[int4range(20,40)], array[int4range(25,35)])', set_lt(array[int4range(20,40)], array[int4range(25,35)]), null),
    ('lte_overlap', 'set_lte(array[int4range(21,40)], array[int4range(1,26)])', set_lte(array[int4range(21,40)], array[int4range(1,26)]), null),
    ('gt_overlap', 'set_gt(array[int4range(1,33)], array[int4range(20,40)])', set_gt(array[int4range(1,33)], array[int4range(20,40)]), null),
    ('gte_overlap', 'set_gte(array[int4range(1,33)], array[int4range(3,40)])', set_gte(array[int4range(1,33)], array[int4range(3,40)]), null),
    ('lt_null', 'set_lt(NULL, array[int4range(3,40)])', set_lt(NULL, array[int4range(3,40)]), null),
    ('lte_null', 'set_lte(NULL, array[int4range(3,40)])', set_lte(NULL, array[int4range(3,40)]), null),
    ('gt_null', 'set_gt(array[int4range(1,33)], NULL)', set_gt(array[int4range(1,33)], NULL), null),
    ('gte_null', 'set_gte(array[int4range(1,33)], NULL)', set_gte(array[int4range(1,33)], NULL), null),
    -- empty cases
    ('lt_empty_true', 'set_lt(array[]::int4range[], array[int4range(25,35)])', set_lt(array[]::int4range[], array[int4range(25,35)]), true),
    ('lte_empty_true', 'set_lte(array[]::int4range[], array[int4range(1,26)])', set_lte(array[]::int4range[], array[int4range(1,26)]), true),
    ('gt_empty_true', 'set_gt(array[int4range(1,33)], array[]::int4range[])', set_gt(array[int4range(1,33)], array[]::int4range[]), true),
    ('gte_empty_true', 'set_gte(array[int4range(1,33)], array[]::int4range[])', set_gte(array[int4range(1,33)], array[]::int4range[]), true),
    ('gt_empty_false', 'set_gt(array[]::int4range[], array[int4range(25,35)])', set_gt(array[]::int4range[], array[int4range(25,35)]), false),
    ('gte_empty_false', 'set_gte(array[]::int4range[], array[int4range(1,26)])', set_gte(array[]::int4range[], array[int4range(1,26)]), false),
    ('lt_empty_false', 'set_lt(array[int4range(1,33)], array[]::int4range[])', set_lt(array[int4range(1,33)], array[]::int4range[]), false),
    ('lte_empty_false', 'set_lte(array[int4range(1,33)], array[]::int4range[])', set_lte(array[int4range(1,33)], array[]::int4range[]), false);