
rtems_tests_PROGRAMS = heapwalk
heapwalk_SOURCES = init.c system.h

dist_rtems_tests_DATA = heapwalk.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(heapwalk_OBJECTS)
LINK_LIBS = $(heapwalk_LDLIBS)

heapwalk$(EXEEXT): $(heapwalk_OBJECTS) $(heapwalk_DEPENDENCIES)
	@rm -f heapwalk$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
