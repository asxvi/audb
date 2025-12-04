#
PGFILEDESC = "C Extension to convert a text to title format"
EXTENSION = c_make_title
MODULE_big = c_make_title
DATA = c_make_title--1.2.sql
OBJS = c_make_title.o	

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)