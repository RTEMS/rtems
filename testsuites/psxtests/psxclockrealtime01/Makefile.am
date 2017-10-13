rtems_tests_PROGRAMS = psxclockrealtime01
psxclockrealtime01_SOURCES = init.c

dist_rtems_tests_DATA = psxclockrealtime01.scn psxclockrealtime01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxclockrealtime01_OBJECTS)
LINK_LIBS = $(psxclockrealtime01_LDLIBS)

psxclockrealtime01$(EXEEXT): $(psxclockrealtime01_OBJECTS) $(psxclockrealtime01_DEPENDENCIES)
	@rm -f psxclockrealtime01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
