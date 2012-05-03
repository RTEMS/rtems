
rtems_tests_PROGRAMS = psx12
psx12_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx12.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx12_OBJECTS)
LINK_LIBS = $(psx12_LDLIBS)

psx12$(EXEEXT): $(psx12_OBJECTS) $(psx12_DEPENDENCIES)
	@rm -f psx12$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
