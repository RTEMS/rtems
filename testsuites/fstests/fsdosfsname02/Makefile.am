rtems_tests_PROGRAMS = fsdosfsname02
fsdosfsname02_SOURCES = init.c

dist_rtems_tests_DATA = fsdosfsname02.scn fsdosfsname02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsdosfsname02_OBJECTS)
LINK_LIBS = $(fsdosfsname02_LDLIBS)

fsdosfsname02$(EXEEXT): $(fsdosfsname02_OBJECTS) $(fsdosfsname02_DEPENDENCIES)
	@rm -f fsdosfsname02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
