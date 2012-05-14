
rtems_tests_PROGRAMS = malloc02
malloc02_SOURCES = init.c

dist_rtems_tests_DATA = malloc02.scn
dist_rtems_tests_DATA += malloc02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloc02_OBJECTS)
LINK_LIBS = $(malloc02_LDLIBS)

malloc02$(EXEEXT): $(malloc02_OBJECTS) $(malloc02_DEPENDENCIES)
	@rm -f malloc02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
