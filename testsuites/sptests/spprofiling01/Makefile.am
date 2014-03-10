rtems_tests_PROGRAMS = spprofiling01
spprofiling01_SOURCES = init.c

dist_rtems_tests_DATA = spprofiling01.scn spprofiling01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spprofiling01_OBJECTS)
LINK_LIBS = $(spprofiling01_LDLIBS)

spprofiling01$(EXEEXT): $(spprofiling01_OBJECTS) $(spprofiling01_DEPENDENCIES)
	@rm -f spprofiling01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
