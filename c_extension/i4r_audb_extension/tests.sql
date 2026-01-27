BEGIN;

-- ===================================
-- Min - basic (certain values only)
-- ===================================
\echo 'Test 1: MIN with 3 certain rows'

CREATE TEMP TABLE test_min_basic (
    colA int4range,
    mult int4range
);

INSERT INTO test_min_basic VALUES
    (int4range(10,20), int4range(1,2)),  
    (int4range(5,15),  int4range(1,2)),  
    (int4range(30,40), int4range(1,2));  


SELECT 
    min(combine_range_mult_min(colA, mult)) as actual,
    int4range(5,15) as expected,
    min(combine_range_mult_min(colA, mult)) = int4range(5,15) as passed;



-- ===================================
-- Min - basic (uncertain + certain values)
-- ===================================
\echo 'Test 2: MIN ignores uncertain rows'

CREATE TEMP TABLE test_min_uncertain (
    colA int4range,
    mult int4range
);

INSERT INTO test_min_uncertain VALUES
    (int4range(10,20), int4range(1,2)),  
    (int4range(1,5),   int4range(0,2)),  
    (int4range(30,40), int4range(1,2));

-- Expected: [10,20) because [1,5) might not exist (mult=[0,2))
-- Manual verification: [1,5) is ignored, min(10, 30) = 10
SELECT 
    min(combine_range_mult_min(colA, mult)) as actual,
    int4range(10,20) as expected,
    min(combine_range_mult_min(colA, mult)) = int4range(10,20) as passed;