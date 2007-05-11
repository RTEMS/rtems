##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp06.exe
sp06_exe_SOURCES = init.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = sp06.scn
dist_rtems_tests_DATA += sp06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp06_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp06_exe_OBJECTS) $(sp06_exe_LDADD)
LINK_LIBS = $(sp06_exe_LDLIBS)

sp06.exe$(EXEEXT): $(sp06_exe_OBJECTS) $(sp06_exe_DEPENDENCIES)
	@rm -f sp06.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
