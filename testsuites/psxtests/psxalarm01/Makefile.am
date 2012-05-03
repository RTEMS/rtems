
rtems_tests_PROGRAMS = psxalarm01
psxalarm01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxalarm01.scn
dist_rtems_tests_DATA += psxalarm01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxalarm01_OBJECTS)
LINK_LIBS = $(psxalarm01_LDLIBS)

psxalarm01$(EXEEXT): $(psxalarm01_OBJECTS) $(psxalarm01_DEPENDENCIES)
	@rm -f psxalarm01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
