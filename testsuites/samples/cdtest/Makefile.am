##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = cdtest.exe
cdtest_exe_SOURCES = init.c main.cc system.h

dist_rtems_tests_DATA = cdtest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

cdtest_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

# AM_CPPFLAGS += -DRTEMS_TEST_IO_STREAM

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(cdtest_exe_OBJECTS) $(cdtest_exe_LDADD)
LINK_LIBS = $(cdtest_exe_LDLIBS)

cdtest.exe$(EXEEXT): $(cdtest_exe_OBJECTS) $(cdtest_exe_DEPENDENCIES)
	@rm -f cdtest.exe$(EXEEXT)
	$(make-cxx-exe)

include $(top_srcdir)/../automake/local.am
