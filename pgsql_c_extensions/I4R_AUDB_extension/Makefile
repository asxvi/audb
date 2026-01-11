#
PGFILEDESC = "C I4R AUDB Extension"
EXTENSION = I4R_AUDB_extension
MODULE_big = I4R_AUDB_extension
DATA = I4R_AUDB_extension--1.1.sql
OBJS = 	\
	arithmetic.o \
	helperFunctions.o \
	I4R_AUDB_extension.o \
	logicalOperators.o

# using postgres@16 for local testing
# PG_CONFIG ?= /usr/local/pgsql16/bin/pg_config
PG_CONFIG = pg_config
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