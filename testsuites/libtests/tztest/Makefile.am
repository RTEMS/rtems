##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = tztest
tztest_SOURCES = init.c

dist_rtems_tests_DATA = tztest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tztest_OBJECTS) $(tztest_LDADD)
LINK_LIBS = $(tztest_LDLIBS)

tztest$(EXEEXT): $(tztest_OBJECTS) $(tztest_DEPENDENCIES)
	@rm -f tztest$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
