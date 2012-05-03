
rtems_tests_PROGRAMS = psx07
psx07_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx07.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx07_OBJECTS)
LINK_LIBS = $(psx07_LDLIBS)

psx07$(EXEEXT): $(psx07_OBJECTS) $(psx07_DEPENDENCIES)
	@rm -f psx07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
