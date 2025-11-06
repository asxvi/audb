CC=gcc
CFLAGS = -Wall -Wextra
TEST_CFLAGS = $(CFLAGS) -I/opt/homebrew/Cellar/cunit/2.1-3/include
TEST_LDFLAGS = -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit

arithmetic: arithmetic.o helperFunctions.o
	$(CC) -o arithmetic arithmetic.o helperFunctions.o

# arithmetic.o: arithmetic.c arithmetic.h helperFunctions.h

# gcc -o test_arithmetic test_arithmetic.c -I/opt/homebrew/Cellar/cunit/2.1-3/include -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit
test_arithmetic: test_arithmetic.o arithmetic.o helperFunctions.o
	$(CC) -o test_arithmetic test_arithmetic.o arithmetic.o helperFunctions.o $(TEST_LDFLAGS)

test_arithmetic.o: test_arithmetic.c arithmetic.h helperFunctions.h
	$(CC) $(TEST_CFLAGS) -c test_arithmetic.c -o test_arithmetic.o

clean:
	rm -f *.o test_arithmetic arithmetic