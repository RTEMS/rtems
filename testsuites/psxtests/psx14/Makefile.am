
rtems_tests_PROGRAMS = psx14
psx14_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx14.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx14_OBJECTS)
LINK_LIBS = $(psx14_LDLIBS)

psx14$(EXEEXT): $(psx14_OBJECTS) $(psx14_DEPENDENCIES)
	@rm -f psx14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
