
rtems_tests_PROGRAMS = malloc04
malloc04_SOURCES = init.c

dist_rtems_tests_DATA = malloc04.scn
dist_rtems_tests_DATA += malloc04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloc04_OBJECTS)
LINK_LIBS = $(malloc04_LDLIBS)

malloc04$(EXEEXT): $(malloc04_OBJECTS) $(malloc04_DEPENDENCIES)
	@rm -f malloc04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
