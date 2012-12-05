rtems_tests_PROGRAMS = fsdosfswrite01
fsdosfswrite01_SOURCES = init.c

dist_rtems_tests_DATA = fsdosfswrite01.scn fsdosfswrite01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsdosfswrite01_OBJECTS)
LINK_LIBS = $(fsdosfswrite01_LDLIBS)

fsdosfswrite01$(EXEEXT): $(fsdosfswrite01_OBJECTS) $(fsdosfswrite01_DEPENDENCIES)
	@rm -f fsdosfswrite01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
