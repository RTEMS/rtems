rtems_tests_PROGRAMS = newlib01
newlib01_SOURCES = init.c

dist_rtems_tests_DATA = newlib01.scn newlib01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(newlib01_OBJECTS)
LINK_LIBS = $(newlib01_LDLIBS)

newlib01$(EXEEXT): $(newlib01_OBJECTS) $(newlib01_DEPENDENCIES)
	@rm -f newlib01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
