##
##  $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = hello
hello_SOURCES = init.c system.h

dist_rtems_tests_DATA = hello.scn
dist_rtems_tests_DATA += hello.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

hello_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(hello_OBJECTS) $(hello_LDADD)
LINK_LIBS = $(hello_LDLIBS)

hello$(EXEEXT): $(hello_OBJECTS) $(hello_DEPENDENCIES)
	@rm -f hello$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
