rtems_tests_PROGRAMS = smpschedule01
smpschedule01_SOURCES = init.c

dist_rtems_tests_DATA = smpschedule01.scn smpschedule01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedule01_OBJECTS)
LINK_LIBS = $(smpschedule01_LDLIBS)

smpschedule01$(EXEEXT): $(smpschedule01_OBJECTS) $(smpschedule01_DEPENDENCIES)
	@rm -f smpschedule01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
