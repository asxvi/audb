CREATE TABLE t_test3_trial_1 (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);

INSERT INTO t_test3_trial_1 (val, mult) VALUES 
    (int4range(88, 99), int4range(2, 4)),
    (int4range(64, 85), int4range(2, 3)),
    (int4range(28, 96), int4range(1, 2)),
    (int4range(96, 99), int4range(4, 5)),
    (int4range(47, 83), int4range(4, 5)),
    (int4range(68, 90), int4range(4, 5)),
    (int4range(27, 66), int4range(3, 4)),
    (int4range(20, 60), int4range(2, 3)),
    (int4range(19, 35), int4range(2, 5)),
    (int4range(51, 95), int4range(4, 5));

