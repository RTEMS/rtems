##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxcleanup.exe
psxcleanup_exe_SOURCES = psxcleanup.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxcleanup.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxcleanup_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcleanup_exe_OBJECTS) $(psxcleanup_exe_LDADD)
LINK_LIBS = $(psxcleanup_exe_LDLIBS)

psxcleanup.exe$(EXEEXT): $(psxcleanup_exe_OBJECTS) $(psxcleanup_exe_DEPENDENCIES)
	@rm -f psxcleanup.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
