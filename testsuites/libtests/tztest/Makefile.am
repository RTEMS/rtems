##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = tztest.exe
tztest_exe_SOURCES = init.c

dist_rtems_tests_DATA = tztest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tztest_exe_OBJECTS) $(tztest_exe_LDADD)
LINK_LIBS = $(tztest_exe_LDLIBS)

tztest.exe$(EXEEXT): $(tztest_exe_OBJECTS) $(tztest_exe_DEPENDENCIES)
	@rm -f tztest.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
