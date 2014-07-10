rtems_tests_PROGRAMS = smpschedaffinity03
smpschedaffinity03_SOURCES = init.c

dist_rtems_tests_DATA = smpschedaffinity03.scn smpschedaffinity03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedaffinity03_OBJECTS)
LINK_LIBS = $(smpschedaffinity03_LDLIBS)

smpschedaffinity03$(EXEEXT): $(smpschedaffinity03_OBJECTS) $(smpschedaffinity03_DEPENDENCIES)
	@rm -f smpschedaffinity03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
