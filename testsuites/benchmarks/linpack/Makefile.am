rtems_tests_PROGRAMS = linpack
linpack_SOURCES = init.c linpack-pc.c
linpack_LDADD = -lm

dist_rtems_tests_DATA = linpack.scn linpack.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CFLAGS += -fno-inline -fno-builtin

LINK_OBJS = $(linpack_OBJECTS) $(linpack_LDADD)
LINK_LIBS = $(linpack_LDLIBS)

linpack$(EXEEXT): $(linpack_OBJECTS) $(linpack_DEPENDENCIES)
	@rm -f linpack$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
