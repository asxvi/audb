CREATE TABLE t_test3_trial_2 (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);

INSERT INTO t_test3_trial_2 (val, mult) VALUES 
    (int4range(53, 83), int4range(2, 5)),
    (int4range(29, 65), NULL),
    (NULL, NULL),
    (int4range(69, 76), NULL),
    (int4range(42, 65), int4range(1, 3)),
    (int4range(72, 76), int4range(2, 5)),
    (int4range(24, 68), NULL),
    (int4range(10, 25), int4range(4, 5)),
    (int4range(9, 37), int4range(3, 5)),
    (int4range(33, 87), int4range(1, 5));

