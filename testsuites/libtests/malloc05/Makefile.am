
rtems_tests_PROGRAMS = malloc05
malloc05_SOURCES = init.c

dist_rtems_tests_DATA = malloc05.scn
dist_rtems_tests_DATA += malloc05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloc05_OBJECTS)
LINK_LIBS = $(malloc05_LDLIBS)

malloc05$(EXEEXT): $(malloc05_OBJECTS) $(malloc05_DEPENDENCIES)
	@rm -f malloc05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
