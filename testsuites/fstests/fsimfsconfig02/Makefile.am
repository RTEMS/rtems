rtems_tests_PROGRAMS = fsimfsconfig02
fsimfsconfig02_SOURCES = init.c

dist_rtems_tests_DATA = fsimfsconfig02.scn fsimfsconfig02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsimfsconfig02_OBJECTS)
LINK_LIBS = $(fsimfsconfig02_LDLIBS)

fsimfsconfig02$(EXEEXT): $(fsimfsconfig02_OBJECTS) $(fsimfsconfig02_DEPENDENCIES)
	@rm -f fsimfsconfig02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
