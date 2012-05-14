
rtems_tests_PROGRAMS = psx08
psx08_SOURCES = init.c task1.c task2.c task3.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx08.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx08_OBJECTS)
LINK_LIBS = $(psx08_LDLIBS)

psx08$(EXEEXT): $(psx08_OBJECTS) $(psx08_DEPENDENCIES)
	@rm -f psx08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
