
rtems_tests_PROGRAMS = malloc03
malloc03_SOURCES = init.c

dist_rtems_tests_DATA = malloc03.scn
dist_rtems_tests_DATA += malloc03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloc03_OBJECTS)
LINK_LIBS = $(malloc03_LDLIBS)

malloc03$(EXEEXT): $(malloc03_OBJECTS) $(malloc03_DEPENDENCIES)
	@rm -f malloc03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
