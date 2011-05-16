AUTOMAKE_OPTIONS = foreign
##AM_LDFLAGS= -static -rdynamic -E -g 
AM_LDFLAGS= -rdynamic -g 
include $(top_srcdir)/Make-rules

#noinst_LIBRARIES = librandom.a
#librandom_a_SOURCES = src/RandomLib/Random.cpp

bin_PROGRAMS = clocking_test
common_SOURCES = \
src/clock_domain.cc \
src/clocked_block.cc \
src/identity_elements.cc

clocking_test_SOURCES = \
 $(common_SOURCES) \
 src/clocking_test.cc

#rigelsim_LDADD = $(LDADD) librandom.a

#if ENABLE_COUCHDB
# rigelsim_SOURCES += src/couchdb/couchdb_simple.cpp
# dramtest_SOURCES += src/couchdb/couchdb_simple.cpp
#endif
