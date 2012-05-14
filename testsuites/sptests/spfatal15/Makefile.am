
rtems_tests_PROGRAMS = spfatal15
spfatal15_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal15.scn
dist_rtems_tests_DATA += spfatal15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal15_OBJECTS)
LINK_LIBS = $(spfatal15_LDLIBS)

spfatal15$(EXEEXT): $(spfatal15_OBJECTS) $(spfatal15_DEPENDENCIES)
	@rm -f spfatal15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
