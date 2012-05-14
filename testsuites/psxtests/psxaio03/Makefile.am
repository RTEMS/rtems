
rtems_tests_PROGRAMS = psxaio03
psxaio03_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxaio03.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxaio03_OBJECTS)
LINK_LIBS = $(psxaio02_LDLIBS)

psxaio03$(EXEEXT): $(psxaio03_OBJECTS) $(psxaio03_DEPENDENCIES)
	@rm -f psxaio03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am