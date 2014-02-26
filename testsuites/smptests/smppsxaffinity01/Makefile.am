
rtems_tests_PROGRAMS = smppsxaffinity01
smppsxaffinity01_SOURCES = init.c

dist_rtems_tests_DATA = smppsxaffinity01.scn
dist_rtems_tests_DATA += smppsxaffinity01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smppsxaffinity01_OBJECTS)
LINK_LIBS = $(smppsxaffinity01_LDLIBS)

smppsxaffinity01$(EXEEXT): $(smppsxaffinity01_OBJECTS) $(smppsxaffinity01_DEPENDENCIES)
	@rm -f smppsxaffinity01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
