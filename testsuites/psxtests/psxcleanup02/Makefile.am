rtems_tests_PROGRAMS = psxcleanup02
psxcleanup02_SOURCES = init.c main.c

dist_rtems_tests_DATA = psxcleanup02.scn psxcleanup02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcleanup02_OBJECTS)
LINK_LIBS = $(psxcleanup02_LDLIBS)

psxcleanup02$(EXEEXT): $(psxcleanup02_OBJECTS) $(psxcleanup02_DEPENDENCIES)
	@rm -f psxcleanup02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
