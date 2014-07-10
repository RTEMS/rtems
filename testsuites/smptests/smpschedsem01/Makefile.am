rtems_tests_PROGRAMS = smpschedsem01
smpschedsem01_SOURCES = init.c

dist_rtems_tests_DATA = smpschedsem01.scn smpschedsem01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschedsem01_OBJECTS)
LINK_LIBS = $(smpschedsem01_LDLIBS)

smpschedsem01$(EXEEXT): $(smpschedsem01_OBJECTS) $(smpschedsem01_DEPENDENCIES)
	@rm -f smpschedsem01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
