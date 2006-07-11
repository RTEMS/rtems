##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask03.exe
itrontask03_exe_SOURCES = init.c preempt.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = itrontask03.scn
dist_rtems_tests_DATA += itrontask03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask03_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask03_exe_OBJECTS) $(itrontask03_exe_LDADD)
LINK_LIBS = $(itrontask03_exe_LDLIBS)

itrontask03.exe$(EXEEXT): $(itrontask03_exe_OBJECTS) $(itrontask03_exe_DEPENDENCIES)
	@rm -f itrontask03.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
