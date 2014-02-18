rtems_tests_PROGRAMS = smpfatal01
smpfatal01_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal01.scn smpfatal01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal01_OBJECTS)
LINK_LIBS = $(smpfatal01_LDLIBS)

smpfatal01$(EXEEXT): $(smpfatal01_OBJECTS) $(smpfatal01_DEPENDENCIES)
	@rm -f smpfatal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
