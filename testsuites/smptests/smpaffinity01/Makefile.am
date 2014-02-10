rtems_tests_PROGRAMS = smpaffinity01
smpaffinity01_SOURCES = init.c

dist_rtems_tests_DATA = smpaffinity01.scn smpaffinity01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpaffinity01_OBJECTS)
LINK_LIBS = $(smpaffinity01_LDLIBS)

smpaffinity01$(EXEEXT): $(smpaffinity01_OBJECTS) $(smpaffinity01_DEPENDENCIES)
	@rm -f smpaffinity01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
