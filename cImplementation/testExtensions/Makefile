#
PGFILEDESC = "C Extension Arithmetic: addition Testing"
EXTENSION = c_range_add
MODULE_big = c_range_add
DATA = c_range_add--1.1.sql
OBJS = arithmetic.o arithmeticExtension.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)