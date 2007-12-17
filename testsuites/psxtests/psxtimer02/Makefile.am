##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxtimer02.exe
psxtimer02_exe_SOURCES = psxtimer.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxtimer02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxtimer02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimer02_exe_OBJECTS) $(psxtimer02_exe_LDADD)
LINK_LIBS = $(psxtimer02_exe_LDLIBS)

psxtimer02.exe$(EXEEXT): $(psxtimer02_exe_OBJECTS) $(psxtimer02_exe_DEPENDENCIES)
	@rm -f psxtimer02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
