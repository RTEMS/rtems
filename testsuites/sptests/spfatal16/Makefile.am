
rtems_tests_PROGRAMS = spfatal16
spfatal16_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal16.scn
dist_rtems_tests_DATA += spfatal16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSEMAPHORES_REMAINING=4

LINK_OBJS = $(spfatal16_OBJECTS)
LINK_LIBS = $(spfatal16_LDLIBS)

spfatal16$(EXEEXT): $(spfatal16_OBJECTS) $(spfatal16_DEPENDENCIES)
	@rm -f spfatal16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
