
rtems_tests_PROGRAMS = psxrdwrv
psxrdwrv_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxrdwrv.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxrdwrv_OBJECTS)
LINK_LIBS = $(psxrdwrv_LDLIBS)

psxrdwrv$(EXEEXT): $(psxrdwrv_OBJECTS) $(psxrdwrv_DEPENDENCIES)
	@rm -f psxrdwrv$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
