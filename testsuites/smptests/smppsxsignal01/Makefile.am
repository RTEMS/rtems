rtems_tests_PROGRAMS = smppsxsignal01
smppsxsignal01_SOURCES = init.c

dist_rtems_tests_DATA = smppsxsignal01.scn smppsxsignal01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smppsxsignal01_OBJECTS)
LINK_LIBS = $(smppsxsignal01_LDLIBS)

smppsxsignal01$(EXEEXT): $(smppsxsignal01_OBJECTS) $(smppsxsignal01_DEPENDENCIES)
	@rm -f smppsxsignal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
