rtems_tests_PROGRAMS = psxglobalcon01
psxglobalcon01_SOURCES = init.cc

dist_rtems_tests_DATA = psxglobalcon01.scn psxglobalcon01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxglobalcon01_OBJECTS)
LINK_LIBS = $(psxglobalcon01_LDLIBS)

psxglobalcon01$(EXEEXT): $(psxglobalcon01_OBJECTS) $(psxglobalcon01_DEPENDENCIES)
	@rm -f psxglobalcon01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
