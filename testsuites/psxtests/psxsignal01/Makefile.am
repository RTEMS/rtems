##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxsignal01.exe
psxsignal01_exe_SOURCES = init.c task1.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxsignal01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxsignal01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsignal01_exe_OBJECTS) $(psxsignal01_exe_LDADD)
LINK_LIBS = $(psxsignal01_exe_LDLIBS)

psxsignal01.exe$(EXEEXT): $(psxsignal01_exe_OBJECTS) $(psxsignal01_exe_DEPENDENCIES)
	@rm -f psxsignal01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
