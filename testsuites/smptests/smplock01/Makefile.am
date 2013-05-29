rtems_tests_PROGRAMS = smplock01
smplock01_SOURCES = init.c

dist_rtems_tests_DATA = smplock01.scn smplock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smplock01_OBJECTS)
LINK_LIBS = $(smplock01_LDLIBS)

smplock01$(EXEEXT): $(smplock01_OBJECTS) $(smplock01_DEPENDENCIES)
	@rm -f smplock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
