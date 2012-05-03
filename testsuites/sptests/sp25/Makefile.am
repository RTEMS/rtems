
rtems_tests_PROGRAMS = sp25
sp25_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp25.scn
dist_rtems_tests_DATA += sp25.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp25_OBJECTS)
LINK_LIBS = $(sp25_LDLIBS)

sp25$(EXEEXT): $(sp25_OBJECTS) $(sp25_DEPENDENCIES)
	@rm -f sp25$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
