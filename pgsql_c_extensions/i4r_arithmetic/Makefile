#
PGFILEDESC = "C I4R Arithmetic Extension"
EXTENSION = i4r_arithmetic
MODULE_big = i4r_arithmetic
DATA = i4r_arithmetic--1.1.sql
OBJS = arithmetic.o arithmeticExtension.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)