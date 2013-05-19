
rtems_tests_PROGRAMS = spsem02
spsem02_SOURCES = init.c

dist_rtems_tests_DATA = spsem02.scn
dist_rtems_tests_DATA += spsem02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem02_OBJECTS)
LINK_LIBS = $(spsem02_LDLIBS)

spsem02$(EXEEXT): $(spsem02_OBJECTS) $(spsem02_DEPENDENCIES)
	@rm -f spsem02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
