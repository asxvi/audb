CREATE TABLE t_test4_trial_1 (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);

INSERT INTO t_test4_trial_1 (val, mult) VALUES 
    (array[int4range(69, 72),int4range(34, 67)], int4range(1, 3)),
    (array[int4range(19, 34),int4range(14, 54)], int4range(4, 5)),
    (array[int4range(90, 98),int4range(80, 81)], int4range(2, 3)),
    (array[NULL,int4range(9, 42)], NULL),
    (array[int4range(71, 94),int4range(29, 75)], int4range(2, 5)),
    (array[int4range(59, 97),NULL], int4range(4, 5)),
    (array[int4range(77, 90),int4range(77, 85)], int4range(1, 3)),
    (array[int4range(7, 36),NULL], int4range(1, 2)),
    (array[int4range(33, 78),int4range(8, 44)], NULL),
    (array[int4range(3, 19),int4range(59, 95)], int4range(4, 5));

