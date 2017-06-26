rtems_tests_PROGRAMS = smpscheduler07
smpscheduler07_SOURCES = init.c ../smpscheduler03/test.c

dist_rtems_tests_DATA = smpscheduler07.scn smpscheduler07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler07_OBJECTS)
LINK_LIBS = $(smpscheduler07_LDLIBS)

smpscheduler07$(EXEEXT): $(smpscheduler07_OBJECTS) $(smpscheduler07_DEPENDENCIES)
	@rm -f smpscheduler07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
