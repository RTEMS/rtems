
rtems_tests_PROGRAMS = sp03
sp03_SOURCES = init.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp03.scn
dist_rtems_tests_DATA += sp03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp03_OBJECTS)
LINK_LIBS = $(sp03_LDLIBS)

sp03$(EXEEXT): $(sp03_OBJECTS) $(sp03_DEPENDENCIES)
	@rm -f sp03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
