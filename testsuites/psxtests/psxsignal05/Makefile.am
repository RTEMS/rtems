
rtems_tests_PROGRAMS = psxsignal05
psxsignal05_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxsignal05.scn
dist_rtems_tests_DATA += psxsignal05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsignal05_OBJECTS)
LINK_LIBS = $(psxsignal05_LDLIBS)

psxsignal05$(EXEEXT): $(psxsignal05_OBJECTS) $(psxsignal05_DEPENDENCIES)
	@rm -f psxsignal05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
