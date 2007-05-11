##
## $Id$
##

MANAGERS = io region semaphore clock

rtems_tests_PROGRAMS = sp16.exe
sp16_exe_SOURCES = init.c task1.c task2.c task3.c task4.c task5.c system.h

dist_rtems_tests_DATA = sp16.scn
dist_rtems_tests_DATA += sp16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp16_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp16_exe_OBJECTS) $(sp16_exe_LDADD)
LINK_LIBS = $(sp16_exe_LDLIBS)

sp16.exe$(EXEEXT): $(sp16_exe_OBJECTS) $(sp16_exe_DEPENDENCIES)
	@rm -f sp16.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
