##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxstat.exe
psxstat_exe_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxstat.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxstat_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxstat_exe_OBJECTS) $(psxstat_exe_LDADD)
LINK_LIBS = $(psxstat_exe_LDLIBS)

psxstat.exe$(EXEEXT): $(psxstat_exe_OBJECTS) $(psxstat_exe_DEPENDENCIES)
	@rm -f psxstat.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
