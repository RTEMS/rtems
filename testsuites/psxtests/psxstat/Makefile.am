
rtems_tests_PROGRAMS = psxstat
psxstat_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxstat.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxstat_OBJECTS)
LINK_LIBS = $(psxstat_LDLIBS)

psxstat$(EXEEXT): $(psxstat_OBJECTS) $(psxstat_DEPENDENCIES)
	@rm -f psxstat$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
