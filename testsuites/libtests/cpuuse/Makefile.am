##
## $Id$
##

MANAGERS = io rate_monotonic semaphore extension

rtems_tests_PROGRAMS = cpuuse.exe
cpuuse_exe_SOURCES = init.c task1.c task2.c task3.c tswitch.c system.h

dist_rtems_tests_DATA = cpuuse.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

cpuuse_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(cpuuse_exe_OBJECTS) $(cpuuse_exe_LDADD)
LINK_LIBS = $(cpuuse_exe_LDLIBS)

cpuuse.exe$(EXEEXT): $(cpuuse_exe_OBJECTS) $(cpuuse_exe_DEPENDENCIES)
	@rm -f cpuuse.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
