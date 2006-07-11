##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxsem01.exe
psxsem01_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxsem01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxsem01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsem01_exe_OBJECTS) $(psxsem01_exe_LDADD)
LINK_LIBS = $(psxsem01_exe_LDLIBS)

psxsem01.exe$(EXEEXT): $(psxsem01_exe_OBJECTS) $(psxsem01_exe_DEPENDENCIES)
	@rm -f psxsem01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
