rtems_tests_PROGRAMS = smpunsupported01
smpunsupported01_SOURCES = init.c

dist_rtems_tests_DATA = smpunsupported01.scn smpunsupported01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpunsupported01_OBJECTS)
LINK_LIBS = $(smpunsupported01_LDLIBS)

smpunsupported01$(EXEEXT): $(smpunsupported01_OBJECTS) $(smpunsupported01_DEPENDENCIES)
	@rm -f smpunsupported01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
