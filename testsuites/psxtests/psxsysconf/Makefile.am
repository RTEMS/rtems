##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxsysconf.exe
psxsysconf_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxsysconf.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxsysconf_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsysconf_exe_OBJECTS) $(psxsysconf_exe_LDADD)
LINK_LIBS = $(psxsysconf_exe_LDLIBS)

psxsysconf.exe$(EXEEXT): $(psxsysconf_exe_OBJECTS) $(psxsysconf_exe_DEPENDENCIES)
	@rm -f psxsysconf.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
