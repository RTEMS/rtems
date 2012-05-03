
rtems_tests_PROGRAMS = psx11
psx11_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx11.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx11_OBJECTS)
LINK_LIBS = $(psx11_LDLIBS)

psx11$(EXEEXT): $(psx11_OBJECTS) $(psx11_DEPENDENCIES)
	@rm -f psx11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
