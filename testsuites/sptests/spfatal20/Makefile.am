
rtems_tests_PROGRAMS = spfatal20
spfatal20_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal20.scn
dist_rtems_tests_DATA += spfatal20.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSEMAPHORES_REMAINING=0

LINK_OBJS = $(spfatal20_OBJECTS)
LINK_LIBS = $(spfatal20_LDLIBS)

spfatal20$(EXEEXT): $(spfatal20_OBJECTS) $(spfatal20_DEPENDENCIES)
	@rm -f spfatal20$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
