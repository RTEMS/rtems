
rtems_tests_PROGRAMS = psxsem01
psxsem01_SOURCES = init.c ../include/pmacros.h \
     ../../support/src/test_support.c

dist_rtems_tests_DATA = psxsem01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsem01_OBJECTS)
LINK_LIBS = $(psxsem01_LDLIBS)

psxsem01$(EXEEXT): $(psxsem01_OBJECTS) $(psxsem01_DEPENDENCIES)
	@rm -f psxsem01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
