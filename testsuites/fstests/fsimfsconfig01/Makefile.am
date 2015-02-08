rtems_tests_PROGRAMS = fsimfsconfig01
fsimfsconfig01_SOURCES = init.c

dist_rtems_tests_DATA = fsimfsconfig01.scn fsimfsconfig01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsimfsconfig01_OBJECTS)
LINK_LIBS = $(fsimfsconfig01_LDLIBS)

fsimfsconfig01$(EXEEXT): $(fsimfsconfig01_OBJECTS) $(fsimfsconfig01_DEPENDENCIES)
	@rm -f fsimfsconfig01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
