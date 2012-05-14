
rtems_tests_PROGRAMS = psxpipe01
psxpipe01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxpipe01.scn
dist_rtems_tests_DATA += psxpipe01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxpipe01_OBJECTS)
LINK_LIBS = $(psxpipe01_LDLIBS)

psxpipe01$(EXEEXT): $(psxpipe01_OBJECTS) $(psxpipe01_DEPENDENCIES)
	@rm -f psxpipe01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
