
rtems_tests_PROGRAMS = sp06
sp06_SOURCES = init.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = sp06.scn
dist_rtems_tests_DATA += sp06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp06_OBJECTS)
LINK_LIBS = $(sp06_LDLIBS)

sp06$(EXEEXT): $(sp06_OBJECTS) $(sp06_DEPENDENCIES)
	@rm -f sp06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
