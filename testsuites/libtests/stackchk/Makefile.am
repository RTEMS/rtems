##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = stackchk.exe
stackchk_exe_SOURCES = blow.c init.c task1.c system.h

dist_rtems_tests_DATA = stackchk.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

stackchk_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(stackchk_exe_OBJECTS) $(stackchk_exe_LDADD)
LINK_LIBS = $(stackchk_exe_LDLIBS)

stackchk.exe$(EXEEXT): $(stackchk_exe_OBJECTS) $(stackchk_exe_DEPENDENCIES)
	@rm -f stackchk.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
