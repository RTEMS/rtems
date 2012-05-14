
rtems_tests_PROGRAMS = monitor
monitor_SOURCES = init.c system.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(monitor_OBJECTS)
LINK_LIBS = $(monitor_LDLIBS)

monitor$(EXEEXT): $(monitor_OBJECTS) $(monitor_DEPENDENCIES)
	@rm -f monitor$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
