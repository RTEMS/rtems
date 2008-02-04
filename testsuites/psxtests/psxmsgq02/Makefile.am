##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxmsgq02.exe
psxmsgq02_exe_SOURCES = init.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c

dist_rtems_tests_DATA = psxmsgq02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxmsgq02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmsgq02_exe_OBJECTS) $(psxmsgq02_exe_LDADD)
LINK_LIBS = $(psxmsgq02_exe_LDLIBS)

psxmsgq02.exe$(EXEEXT): $(psxmsgq02_exe_OBJECTS) $(psxmsgq02_exe_DEPENDENCIES)
	@rm -f psxmsgq02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
