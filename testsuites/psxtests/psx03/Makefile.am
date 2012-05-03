
rtems_tests_PROGRAMS = psx03
psx03_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx03.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx03_OBJECTS)
LINK_LIBS = $(psx03_LDLIBS)

psx03$(EXEEXT): $(psx03_OBJECTS) $(psx03_DEPENDENCIES)
	@rm -f psx03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
