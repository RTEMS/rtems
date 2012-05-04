rtems_tests_PROGRAMS = devfs03
devfs03_SOURCES = init.c

dist_rtems_tests_DATA = devfs03.scn
dist_rtems_tests_DATA += devfs03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(devfs03_OBJECTS)
LINK_LIBS = $(devfs03_LDLIBS)

devfs03$(EXEEXT): $(devfs03_OBJECTS) $(devfs03_DEPENDENCIES)
	@rm -f devfs03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
