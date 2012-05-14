
rtems_tests_PROGRAMS = psx13
psx13_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psx13.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx13_OBJECTS)
LINK_LIBS = $(psx13_LDLIBS)

psx13$(EXEEXT): $(psx13_OBJECTS) $(psx13_DEPENDENCIES)
	@rm -f psx13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
