
rtems_tests_PROGRAMS = devfs01
devfs01_SOURCES = init.c

dist_rtems_tests_DATA = devfs01.scn
dist_rtems_tests_DATA += devfs01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(devfs01_OBJECTS)
LINK_LIBS = $(devfs01_LDLIBS)

devfs01$(EXEEXT): $(devfs01_OBJECTS) $(devfs01_DEPENDENCIES)
	@rm -f devfs01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
