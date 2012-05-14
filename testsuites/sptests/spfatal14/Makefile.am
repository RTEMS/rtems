
rtems_tests_PROGRAMS = spfatal14
spfatal14_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/consume_sems.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal14.scn
dist_rtems_tests_DATA += spfatal14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal14_OBJECTS)
LINK_LIBS = $(spfatal14_LDLIBS)

spfatal14$(EXEEXT): $(spfatal14_OBJECTS) $(spfatal14_DEPENDENCIES)
	@rm -f spfatal14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
