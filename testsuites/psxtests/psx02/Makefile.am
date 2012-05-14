
rtems_tests_PROGRAMS = psx02
psx02_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx02_OBJECTS)
LINK_LIBS = $(psx02_LDLIBS)

psx02$(EXEEXT): $(psx02_OBJECTS) $(psx02_DEPENDENCIES)
	@rm -f psx02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
