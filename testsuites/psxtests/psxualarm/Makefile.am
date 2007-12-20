##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxualarm.exe
psxualarm_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxualarm.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxualarm_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxualarm_exe_OBJECTS) $(psxualarm_exe_LDADD)
LINK_LIBS = $(psxulaarm_exe_LDLIBS)

psxualarm.exe$(EXEEXT): $(psxualarm_exe_OBJECTS) $(psxualarm_exe_DEPENDENCIES)
	@rm -f psxualarm.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
