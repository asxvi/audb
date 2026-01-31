CREATE TABLE t_test4_trial_2 (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);

INSERT INTO t_test4_trial_2 (val, mult) VALUES 
    (array[int4range(88, 96),int4range(60, 74)], int4range(4, 5)),
    (array[int4range(65, 83),int4range(85, 91)], int4range(1, 2)),
    (array[int4range(45, 51),NULL], int4range(3, 5)),
    (array[int4range(16, 53),NULL], int4range(3, 5)),
    (array[int4range(4, 71),int4range(19, 20)], int4range(1, 2)),
    (array[int4range(88, 95),int4range(21, 30)], NULL),
    (array[int4range(94, 95),int4range(24, 86)], int4range(1, 2)),
    (array[int4range(99, 100),int4range(74, 87)], int4range(3, 4)),
    (array[int4range(83, 97),NULL], int4range(3, 4)),
    (array[int4range(37, 58),int4range(86, 88)], int4range(4, 5));

