
rtems_tests_PROGRAMS = spfatal19
spfatal19_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal19.scn
dist_rtems_tests_DATA += spfatal19.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSEMAPHORES_REMAINING=1

LINK_OBJS = $(spfatal19_OBJECTS)
LINK_LIBS = $(spfatal19_LDLIBS)

spfatal19$(EXEEXT): $(spfatal19_OBJECTS) $(spfatal19_DEPENDENCIES)
	@rm -f spfatal19$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
