##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxenosys.exe
psxenosys_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxenosys.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxenosys_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxenosys_exe_OBJECTS) $(psxenosys_exe_LDADD)
LINK_LIBS = $(psxenosys_exe_LDLIBS)

psxenosys.exe$(EXEEXT): $(psxenosys_exe_OBJECTS) $(psxenosys_exe_DEPENDENCIES)
	@rm -f psxenosys.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
