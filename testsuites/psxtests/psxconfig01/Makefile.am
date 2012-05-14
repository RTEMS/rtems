
rtems_tests_PROGRAMS = psxconfig01
psxconfig01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxconfig01.scn
dist_rtems_tests_DATA += psxconfig01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxconfig01_OBJECTS)
LINK_LIBS = $(psxconfig01_LDLIBS)

psxconfig01$(EXEEXT): $(psxconfig01_OBJECTS) \
    $(psxconfig01_DEPENDENCIES)
	@rm -f psxconfig01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
