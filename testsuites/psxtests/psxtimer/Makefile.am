##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxtimer.exe
psxtimer_exe_SOURCES = psxtimer.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxtimer.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxtimer_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimer_exe_OBJECTS) $(psxtimer_exe_LDADD)
LINK_LIBS = $(psxtimer_exe_LDLIBS)

psxtimer.exe$(EXEEXT): $(psxtimer_exe_OBJECTS) $(psxtimer_exe_DEPENDENCIES)
	@rm -f psxtimer.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
