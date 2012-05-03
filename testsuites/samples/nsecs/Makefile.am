
rtems_tests_PROGRAMS = nsecs
nsecs_SOURCES = init.c empty.c

dist_rtems_tests_DATA = nsecs.scn
dist_rtems_tests_DATA += nsecs.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(nsecs_OBJECTS)
LINK_LIBS = $(nsecs_LDLIBS)

nsecs$(EXEEXT): $(nsecs_OBJECTS) $(nsecs_DEPENDENCIES)
	@rm -f nsecs$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
