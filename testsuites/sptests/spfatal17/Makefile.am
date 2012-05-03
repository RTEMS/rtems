
rtems_tests_PROGRAMS = spfatal17
spfatal17_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal17.scn
dist_rtems_tests_DATA += spfatal17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSEMAPHORES_REMAINING=3

LINK_OBJS = $(spfatal17_OBJECTS)
LINK_LIBS = $(spfatal17_LDLIBS)

spfatal17$(EXEEXT): $(spfatal17_OBJECTS) $(spfatal17_DEPENDENCIES)
	@rm -f spfatal17$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
