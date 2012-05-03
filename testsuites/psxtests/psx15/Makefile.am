
rtems_tests_PROGRAMS = psx15
psx15_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psx15.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx15_OBJECTS)
LINK_LIBS = $(psx15_LDLIBS)

psx15$(EXEEXT): $(psx15_OBJECTS) $(psx15_DEPENDENCIES)
	@rm -f psx15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
