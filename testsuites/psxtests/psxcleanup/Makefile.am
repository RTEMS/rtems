
rtems_tests_PROGRAMS = psxcleanup
psxcleanup_SOURCES = psxcleanup.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxcleanup.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcleanup_OBJECTS)
LINK_LIBS = $(psxcleanup_LDLIBS)

psxcleanup$(EXEEXT): $(psxcleanup_OBJECTS) $(psxcleanup_DEPENDENCIES)
	@rm -f psxcleanup$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
