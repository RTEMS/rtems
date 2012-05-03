
rtems_tests_PROGRAMS = sp71
sp71_SOURCES = init.c

dist_rtems_tests_DATA = sp71.scn
dist_rtems_tests_DATA += sp71.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp71_OBJECTS)
LINK_LIBS = $(sp71_LDLIBS)

sp71$(EXEEXT): $(sp71_OBJECTS) $(sp71_DEPENDENCIES)
	@rm -f sp71$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
