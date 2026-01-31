CREATE TABLE t_test3_trial_1 (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);

INSERT INTO t_test3_trial_1 (val, mult) VALUES 
    (int4range(72, 86), int4range(4, 5)),
    (int4range(78, 93), int4range(3, 4)),
    (int4range(64, 72), int4range(1, 3)),
    (int4range(42, 72), int4range(3, 4)),
    (int4range(76, 94), int4range(1, 3)),
    (int4range(9, 100), int4range(3, 5)),
    (int4range(60, 79), NULL),
    (int4range(99, 100), int4range(2, 3)),
    (int4range(53, 81), int4range(4, 5)),
    (int4range(36, 81), int4range(1, 2));

