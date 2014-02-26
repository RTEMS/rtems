rtems_tests_PROGRAMS = smppsxaffinity02
smppsxaffinity02_SOURCES = init.c

dist_rtems_tests_DATA = smppsxaffinity02.scn smppsxaffinity02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smppsxaffinity02_OBJECTS)
LINK_LIBS = $(smppsxaffinity02_LDLIBS)

smppsxaffinity02$(EXEEXT): $(smppsxaffinity02_OBJECTS) $(smppsxaffinity02_DEPENDENCIES)
	@rm -f smppsxaffinity02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
