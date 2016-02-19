
rtems_tests_PROGRAMS = psxconcurrency01
psxconcurrency01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxconcurrency01.scn
dist_rtems_tests_DATA += psxconcurrency01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxconcurrency01_OBJECTS)
LINK_LIBS = $(psxconcurrency01_LDLIBS)

psxconcurrency01$(EXEEXT): $(psxconcurrency01_OBJECTS) $(psxconcurrency01_DEPENDENCIES)
	@rm -f psxconcurrency01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
