##
##  $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = hello.exe
hello_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = hello.scn
dist_rtems_tests_DATA += hello.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

hello_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(hello_exe_OBJECTS) $(hello_exe_LDADD)
LINK_LIBS = $(hello_exe_LDLIBS)

hello.exe$(EXEEXT): $(hello_exe_OBJECTS) $(hello_exe_DEPENDENCIES)
	@rm -f hello.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
