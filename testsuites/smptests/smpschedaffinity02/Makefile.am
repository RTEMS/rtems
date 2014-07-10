rtems_tests_PROGRAMS = smpschedaffinity02
smpschedaffinity02_SOURCES = init.c

dist_rtems_tests_DATA = smpschedaffinity02.scn smpschedaffinity02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedaffinity02_OBJECTS)
LINK_LIBS = $(smpschedaffinity02_LDLIBS)

smpschedaffinity02$(EXEEXT): $(smpschedaffinity02_OBJECTS) $(smpschedaffinity02_DEPENDENCIES)
	@rm -f smpschedaffinity02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
