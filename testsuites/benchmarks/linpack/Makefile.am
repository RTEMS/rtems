rtems_tests_PROGRAMS = linpack
linpack_SOURCES = init.c

dist_rtems_tests_DATA = linpack.scn linpack.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(linpack_OBJECTS)
LINK_LIBS = $(linpack_LDLIBS)

linpack$(EXEEXT): $(linpack_OBJECTS) $(linpack_DEPENDENCIES)
	@rm -f linpack$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
