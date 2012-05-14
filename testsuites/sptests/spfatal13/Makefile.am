
rtems_tests_PROGRAMS = spfatal13
spfatal13_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h \
    testcase.h

dist_rtems_tests_DATA = spfatal13.scn
dist_rtems_tests_DATA += spfatal13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal13_OBJECTS)
LINK_LIBS = $(spfatal13_LDLIBS)

spfatal13$(EXEEXT): $(spfatal13_OBJECTS) $(spfatal13_DEPENDENCIES)
	@rm -f spfatal13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
