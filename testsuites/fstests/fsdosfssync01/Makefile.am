rtems_tests_PROGRAMS = fsdosfssync01
fsdosfssync01_SOURCES = init.c

dist_rtems_tests_DATA = fsdosfssync01.scn fsdosfssync01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsdosfssync01_OBJECTS)
LINK_LIBS = $(fsdosfssync01_LDLIBS)

fsdosfssync01$(EXEEXT): $(fsdosfssync01_OBJECTS) $(fsdosfssync01_DEPENDENCIES)
	@rm -f fsdosfssync01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
