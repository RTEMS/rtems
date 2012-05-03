
rtems_tests_PROGRAMS = psxmsgq01
psxmsgq01_SOURCES = init.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c


dist_rtems_tests_DATA = psxmsgq01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmsgq01_OBJECTS)
LINK_LIBS = $(psxmsgq01_LDLIBS)

psxmsgq01$(EXEEXT): $(psxmsgq01_OBJECTS) $(psxmsgq01_DEPENDENCIES)
	@rm -f psxmsgq01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
