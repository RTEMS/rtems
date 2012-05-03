
rtems_tests_PROGRAMS = psxspin01
psxspin01_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxspin01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxspin01_OBJECTS)
LINK_LIBS = $(psxspin01_LDLIBS)

psxspin01$(EXEEXT): $(psxspin01_OBJECTS) \
    $(psxspin01_DEPENDENCIES)
	@rm -f psxspin01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
