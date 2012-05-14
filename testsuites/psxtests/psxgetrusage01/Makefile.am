
rtems_tests_PROGRAMS = psxgetrusage01
psxgetrusage01_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = psxgetrusage01.scn
dist_rtems_tests_DATA += psxgetrusage01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxgetrusage01_OBJECTS)
LINK_LIBS = $(psxgetrusage01_LDLIBS)

psxgetrusage01$(EXEEXT): $(psxgetrusage01_OBJECTS) $(psxgetrusage01_DEPENDENCIES)
	@rm -f psxgetrusage01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
