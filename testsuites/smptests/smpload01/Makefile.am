rtems_tests_PROGRAMS = smpload01
smpload01_SOURCES = init.c

dist_rtems_tests_DATA = smpload01.scn smpload01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpload01_OBJECTS)
LINK_LIBS = $(smpload01_LDLIBS)

smpload01$(EXEEXT): $(smpload01_OBJECTS) $(smpload01_DEPENDENCIES)
	@rm -f smpload01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
