rtems_tests_PROGRAMS = devfs04
devfs04_SOURCES = init.c test_driver.c
devfs04_SOURCES += test_driver.h

dist_rtems_tests_DATA = devfs04.scn
dist_rtems_tests_DATA += devfs04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(devfs04_OBJECTS)
LINK_LIBS = $(devfs04_LDLIBS)

devfs04$(EXEEXT): $(devfs04_OBJECTS) $(devfs04_DEPENDENCIES)
	@rm -f devfs04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
