rtems_tests_PROGRAMS = devfs02
devfs02_SOURCES = init.c

dist_rtems_tests_DATA = devfs02.scn
dist_rtems_tests_DATA += devfs02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(devfs02_OBJECTS)
LINK_LIBS = $(devfs02_LDLIBS)

devfs02$(EXEEXT): $(devfs02_OBJECTS) $(devfs02_DEPENDENCIES)
	@rm -f devfs02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
