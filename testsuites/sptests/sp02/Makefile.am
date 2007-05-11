##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp02.exe
sp02_exe_SOURCES = init.c task1.c task2.c task3.c preempt.c system.h

dist_rtems_tests_DATA = sp02.scn
dist_rtems_tests_DATA += sp02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp02_exe_OBJECTS) $(sp02_exe_LDADD)
LINK_LIBS = $(sp02_exe_LDLIBS)

sp02.exe$(EXEEXT): $(sp02_exe_OBJECTS) $(sp02_exe_DEPENDENCIES)
	@rm -f sp02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
