rtems_tests_PROGRAMS = smpscheduler06
smpscheduler06_SOURCES = init.c ../smpscheduler03/test.c

dist_rtems_tests_DATA = smpscheduler06.scn smpscheduler06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler06_OBJECTS)
LINK_LIBS = $(smpscheduler06_LDLIBS)

smpscheduler06$(EXEEXT): $(smpscheduler06_OBJECTS) $(smpscheduler06_DEPENDENCIES)
	@rm -f smpscheduler06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
