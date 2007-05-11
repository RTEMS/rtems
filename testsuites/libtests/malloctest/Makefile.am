##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = malloctest.exe
malloctest_exe_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = malloctest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

malloctest_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloctest_exe_OBJECTS) $(malloctest_exe_LDADD)
LINK_LIBS = $(malloctest_exe_LDLIBS)

malloctest.exe$(EXEEXT): $(malloctest_exe_OBJECTS) $(malloctest_exe_DEPENDENCIES)
	@rm -f malloctest.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
