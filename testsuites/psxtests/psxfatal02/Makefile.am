##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxfatal02.exe
psxfatal02_exe_SOURCES = ../psxfatal_support/init.c ../psxfatal_support/system.h testcase.h

dist_rtems_tests_DATA = psxfatal02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxfatal02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfatal02_exe_OBJECTS) $(psxfatal02_exe_LDADD)
LINK_LIBS = $(psxfatal02_exe_LDLIBS)

psxfatal02.exe$(EXEEXT): $(psxfatal02_exe_OBJECTS) $(psxfatal02_exe_DEPENDENCIES)
	@rm -f psxfatal02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
