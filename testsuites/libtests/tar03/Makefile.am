rtems_tests_PROGRAMS = tar03
tar03_SOURCES = init.c

dist_rtems_tests_DATA = tar03.scn
dist_rtems_tests_DATA += tar03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tar03_OBJECTS)
LINK_LIBS = $(tar03_LDLIBS)

tar03$(EXEEXT): $(tar03_OBJECTS) $(tar03_DEPENDENCIES)
	@rm -f tar03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
