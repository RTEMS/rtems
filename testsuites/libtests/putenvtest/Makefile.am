##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = putenvtest.exe
putenvtest_exe_SOURCES = init.c

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

putenvtest_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(putenvtest_exe_OBJECTS) $(putenvtest_exe_LDADD)
LINK_LIBS = $(putenvtest_exe_LDLIBS)

putenvtest.exe$(EXEEXT): $(putenvtest_exe_OBJECTS) $(putenvtest_exe_DEPENDENCIES)
	@rm -f putenvtest.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
