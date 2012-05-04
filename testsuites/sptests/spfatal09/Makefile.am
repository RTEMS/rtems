rtems_tests_PROGRAMS = spfatal09
spfatal09_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h \
    testcase.h

dist_rtems_tests_DATA = spfatal09.scn
dist_rtems_tests_DATA += spfatal09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal09_OBJECTS)
LINK_LIBS = $(spfatal09_LDLIBS)

spfatal09$(EXEEXT): $(spfatal09_OBJECTS) $(spfatal09_DEPENDENCIES)
	@rm -f spfatal09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
