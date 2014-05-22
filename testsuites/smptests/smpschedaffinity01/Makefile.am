rtems_tests_PROGRAMS = smpschedaffinity01
smpschedaffinity01_SOURCES = init.c

dist_rtems_tests_DATA = smpschedaffinity01.scn smpschedaffinity01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedaffinity01_OBJECTS)
LINK_LIBS = $(smpschedaffinity01_LDLIBS)

smpschedaffinity01$(EXEEXT): $(smpschedaffinity01_OBJECTS) $(smpschedaffinity01_DEPENDENCIES)
	@rm -f smpschedaffinity01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
