
rtems_tests_PROGRAMS = psx01
psx01_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx01_OBJECTS)
LINK_LIBS = $(psx01_LDLIBS)

psx01$(EXEEXT): $(psx01_OBJECTS) $(psx01_DEPENDENCIES)
	@rm -f psx01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
