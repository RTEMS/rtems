
rtems_tests_PROGRAMS = sp75
sp75_SOURCES = init.c

dist_rtems_tests_DATA = sp75.scn
dist_rtems_tests_DATA += sp75.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp75_OBJECTS)
LINK_LIBS = $(sp75_LDLIBS)

sp75$(EXEEXT): $(sp75_OBJECTS) $(sp75_DEPENDENCIES)
	@rm -f sp75$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
