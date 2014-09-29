
rtems_tests_PROGRAMS = top
top_SOURCES = init.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = top.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(top_OBJECTS)
LINK_LIBS = $(top_LDLIBS)

top$(EXEEXT): $(top_OBJECTS) $(top_DEPENDENCIES)
	@rm -f top$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
