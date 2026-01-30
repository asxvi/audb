CREATE TABLE t_test4_trial_2 (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);

INSERT INTO t_test4_trial_2 (val, mult) VALUES 
    (array[int4range(51, 74),int4range(54, 94)], NULL),
    (array[int4range(77, 87),NULL], int4range(4, 5)),
    (array[int4range(60, 90),NULL], NULL),
    (array[int4range(44, 99),int4range(46, 54)], int4range(3, 5)),
    (array[int4range(4, 11),int4range(63, 97)], int4range(2, 4)),
    (array[int4range(63, 65),int4range(19, 71)], int4range(2, 4)),
    (array[int4range(86, 90),int4range(85, 100)], int4range(4, 5)),
    (array[int4range(17, 18),int4range(26, 38)], int4range(3, 4)),
    (array[int4range(24, 93),int4range(85, 99)], int4range(4, 5)),
    (array[int4range(80, 84),int4range(26, 43)], NULL);

