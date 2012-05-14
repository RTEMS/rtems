
rtems_tests_PROGRAMS = psxspin02
psxspin02_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxspin02.scn
dist_rtems_tests_DATA += psxspin02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxspin02_OBJECTS)
LINK_LIBS = $(psxspin02_LDLIBS)

psxspin02$(EXEEXT): $(psxspin02_OBJECTS) \
    $(psxspin02_DEPENDENCIES)
	@rm -f psxspin02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
