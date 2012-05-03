
rtems_tests_PROGRAMS = psx10
psx10_SOURCES = init.c task.c task2.c task3.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx10.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx10_OBJECTS)
LINK_LIBS = $(psx10_LDLIBS)

psx10$(EXEEXT): $(psx10_OBJECTS) $(psx10_DEPENDENCIES)
	@rm -f psx10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
