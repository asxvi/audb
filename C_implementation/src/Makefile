CC=gcc
CFLAGS = -Wall -Wextra #-I/usr/local/pgsql16/include/server
# PGFLAGS = /usr/local/pgsql16/include/server
TEST_CFLAGS = $(CFLAGS) -I/opt/homebrew/Cellar/cunit/2.1-3/include
TEST_LDFLAGS = -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

arithmetic: arithmetic.o helperFunctions.o logicalOperators.o
	$(CC) -o arithmetic arithmetic.o helperFunctions.o logicalOperators.o $(CFLAGS)

arithmetic.o: arithmetic.c arithmetic.h helperFunctions.h logicalOperators.h 

# gcc -o test_arithmetic test_arithmetic.c -I/opt/homebrew/Cellar/cunit/2.1-3/include -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit
test_arithmetic: test_arithmetic.o arithmetic.o helperFunctions.o logicalOperators.o
	$(CC) -o test_arithmetic test_arithmetic.o arithmetic.o helperFunctions.o logicalOperators.o $(TEST_LDFLAGS)

test_arithmetic.o: test_arithmetic.c arithmetic.h helperFunctions.h logicalOperators.h
	$(CC) $(TEST_CFLAGS) -c test_arithmetic.c -o test_arithmetic.o logicalOperators.o

clean:
	rm -f *.o test_arithmetic arithmetic

