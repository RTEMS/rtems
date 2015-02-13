rtems_tests_PROGRAMS = fsimfsconfig03
fsimfsconfig03_SOURCES = init.c

dist_rtems_tests_DATA = fsimfsconfig03.scn fsimfsconfig03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsimfsconfig03_OBJECTS)
LINK_LIBS = $(fsimfsconfig03_LDLIBS)

fsimfsconfig03$(EXEEXT): $(fsimfsconfig03_OBJECTS) $(fsimfsconfig03_DEPENDENCIES)
	@rm -f fsimfsconfig03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
