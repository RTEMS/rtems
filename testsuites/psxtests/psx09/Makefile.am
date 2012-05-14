
rtems_tests_PROGRAMS = psx09
psx09_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx09.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx09_OBJECTS)
LINK_LIBS = $(psx09_LDLIBS)

psx09$(EXEEXT): $(psx09_OBJECTS) $(psx09_DEPENDENCIES)
	@rm -f psx09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
