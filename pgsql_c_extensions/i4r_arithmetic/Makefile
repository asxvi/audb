#
PGFILEDESC = "C I4R Arithmetic Extension"
EXTENSION = i4r_arithmetic
MODULE_big = i4r_arithmetic
DATA = i4r_arithmetic--1.1.sql
OBJS = 	\
	arithmetic.o \
	arithmeticExtension.o \
	helperFunctions.o \
	logicalOperators.o
# does order matter when working with macros. ex define malloc palloc before #includes would modify each malloc to become palloc right

# using postgres@16 for local testing
# PG_CONFIG = pg_config
PG_CONFIG = /usr/local/pgsql16/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# ifdef USE_PGXS
# PG_CONFIG = pg_config
# PGXS := $(shell $(PG_CONFIG) --pgxs)
# include $(PGXS)
# else
# subdir = contrib/amcheck
# top_builddir = ../..
# include $(top_builddir)/src/Makefile.global
# include $(top_srcdir)/contrib/contrib-global.mk
# endif