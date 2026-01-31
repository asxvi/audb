CREATE TABLE t_test3_trial_2 (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);

INSERT INTO t_test3_trial_2 (val, mult) VALUES 
    (int4range(38, 48), int4range(2, 3)),
    (int4range(9, 82), NULL),
    (int4range(79, 95), int4range(2, 5)),
    (int4range(76, 84), int4range(4, 5)),
    (int4range(52, 94), int4range(4, 5)),
    (NULL, int4range(2, 4)),
    (int4range(68, 97), int4range(2, 3)),
    (int4range(54, 63), int4range(1, 2)),
    (int4range(63, 95), int4range(2, 5)),
    (int4range(96, 99), int4range(1, 2));

