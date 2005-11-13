##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxtime.exe
psxtime_exe_SOURCES = main.c test.c

scndir = $(rtems_testsdir)
dist_scn_DATA = psxtime.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxtime_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtime_exe_OBJECTS) $(psxtime_exe_LDADD)
LINK_LIBS = $(psxtime_exe_LDLIBS)

psxtime.exe$(EXEEXT): $(psxtime_exe_OBJECTS) $(psxtime_exe_DEPENDENCIES)
	@rm -f psxtime.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
