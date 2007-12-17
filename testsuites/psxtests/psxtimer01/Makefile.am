##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxtimer01.exe
psxtimer01_exe_SOURCES = psxtimer.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxtimer01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxtimer01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimer01_exe_OBJECTS) $(psxtimer01_exe_LDADD)
LINK_LIBS = $(psxtimer01_exe_LDLIBS)

psxtimer01.exe$(EXEEXT): $(psxtimer01_exe_OBJECTS) $(psxtimer01_exe_DEPENDENCIES)
	@rm -f psxtimer01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
