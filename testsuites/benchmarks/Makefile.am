#
# Benchmarks
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

benchmarks =
benchmark_screens =
benchmark_docs =

support_includes = -I$(top_srcdir)/../support/include

if TEST_dhrystone
benchmarks += dhrystone
benchmark_screens += dhrystone/dhrystone.scn
benchmark_docs += dhrystone/dhrystone.doc
dhrystone_SOURCES = dhrystone/init.c dhrystone/dhry_1.c \
	dhrystone/dhry_2.c
dhrystone_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dhrystone) \
	$(support_includes)
endif

if TEST_linpack
benchmarks += linpack
benchmark_screens += linpack/linpack.scn
benchmark_docs += linpack/linpack.doc
linpack_SOURCES = linpack/init.c linpack/linpack-pc.c
linpack_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_linpack) \
	$(support_includes)
linpack_LDADD = -lm $(LDADD)
endif

if TEST_whetstone
benchmarks += whetstone
benchmark_screens += whetstone/whetstone.scn
benchmark_docs += whetstone/whetstone.doc
whetstone_SOURCES = whetstone/init.c whetstone/whetstone.c
whetstone_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_whetstone) \
	$(support_includes)
whetstone_LDADD = -lm $(LDADD)
endif

noinst_PROGRAMS = $(benchmarks)
