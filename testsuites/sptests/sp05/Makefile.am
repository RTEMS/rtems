##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp05.exe
sp05_exe_SOURCES = init.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = sp05.scn
dist_rtems_tests_DATA += sp05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp05_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp05_exe_OBJECTS) $(sp05_exe_LDADD)
LINK_LIBS = $(sp05_exe_LDLIBS)

sp05.exe$(EXEEXT): $(sp05_exe_OBJECTS) $(sp05_exe_DEPENDENCIES)
	@rm -f sp05.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
