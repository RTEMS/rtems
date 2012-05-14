
rtems_tests_PROGRAMS = psx06
psx06_SOURCES = init.c task.c task2.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx06.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx06_OBJECTS)
LINK_LIBS = $(psx06_LDLIBS)

psx06$(EXEEXT): $(psx06_OBJECTS) $(psx06_DEPENDENCIES)
	@rm -f psx06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
