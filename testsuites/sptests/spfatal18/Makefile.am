
rtems_tests_PROGRAMS = spfatal18
spfatal18_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal18.scn
dist_rtems_tests_DATA += spfatal18.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSEMAPHORES_REMAINING=2

LINK_OBJS = $(spfatal18_OBJECTS)
LINK_LIBS = $(spfatal18_LDLIBS)

spfatal18$(EXEEXT): $(spfatal18_OBJECTS) $(spfatal18_DEPENDENCIES)
	@rm -f spfatal18$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
