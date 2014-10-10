rtems_tests_PROGRAMS = psxglobalcon02
psxglobalcon02_SOURCES = init.cc

dist_rtems_tests_DATA = psxglobalcon02.scn psxglobalcon02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxglobalcon02_OBJECTS)
LINK_LIBS = $(psxglobalcon02_LDLIBS)

psxglobalcon02$(EXEEXT): $(psxglobalcon02_OBJECTS) $(psxglobalcon02_DEPENDENCIES)
	@rm -f psxglobalcon02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
