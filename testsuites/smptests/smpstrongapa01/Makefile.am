rtems_tests_PROGRAMS = smpstrongapa01
smpstrongapa01_SOURCES = init.c

dist_rtems_tests_DATA = smpstrongapa01.scn smpstrongapa01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpstrongapa01_OBJECTS)
LINK_LIBS = $(smpstrongapa01_LDLIBS)

smpstrongapa01$(EXEEXT): $(smpstrongapa01_OBJECTS) $(smpstrongapa01_DEPENDENCIES)
	@rm -f smpstrongapa01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
