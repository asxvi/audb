CREATE TABLE t_test4_trial_1 (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);

INSERT INTO t_test4_trial_1 (val, mult) VALUES 
    (array[int4range(41, 97),int4range(69, 79)], int4range(3, 5)),
    (array[int4range(79, 88),int4range(32, 72)], int4range(1, 2)),
    (array[int4range(95, 99),int4range(18, 52)], int4range(1, 4)),
    (array[int4range(55, 68),int4range(93, 98)], int4range(1, 2)),
    (array[int4range(92, 93),int4range(86, 98)], NULL),
    (array[int4range(97, 100),int4range(64, 87)], int4range(2, 4)),
    (array[int4range(60, 92),NULL], int4range(2, 5)),
    (array[int4range(71, 76),int4range(8, 48)], int4range(1, 2)),
    (array[int4range(27, 58),int4range(23, 63)], NULL),
    (array[int4range(21, 33),NULL], int4range(2, 4));

