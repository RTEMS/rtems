rtems_tests_PROGRAMS = smpschedaffinity04
smpschedaffinity04_SOURCES = init.c

dist_rtems_tests_DATA = smpschedaffinity04.scn smpschedaffinity04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedaffinity04_OBJECTS)
LINK_LIBS = $(smpschedaffinity04_LDLIBS)

smpschedaffinity04$(EXEEXT): $(smpschedaffinity04_OBJECTS) $(smpschedaffinity04_DEPENDENCIES)
	@rm -f smpschedaffinity04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
