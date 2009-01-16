##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spwatchdog.exe
spwatchdog_exe_SOURCES = init.c task1.c system.h prtime.c delay.c

dist_rtems_tests_DATA = spwatchdog.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spwatchdog_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spwatchdog_exe_OBJECTS) $(spwatchdog_exe_LDADD)
LINK_LIBS = $(spwatchdog_exe_LDLIBS)

spwatchdog.exe$(EXEEXT): $(spwatchdog_exe_OBJECTS) $(spwatchdog_exe_DEPENDENCIES)
	@rm -f spwatchdog.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
