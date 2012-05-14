
rtems_tests_PROGRAMS = psxsignal03
psxsignal03_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxsignal03.scn
dist_rtems_tests_DATA += psxsignal03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_USER_SIGNALS_PROCESS

LINK_OBJS = $(psxsignal03_OBJECTS)
LINK_LIBS = $(psxsignal03_LDLIBS)

psxsignal03$(EXEEXT): $(psxsignal03_OBJECTS) $(psxsignal03_DEPENDENCIES)
	@rm -f psxsignal03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
