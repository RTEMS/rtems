##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxmsgq01.exe
psxmsgq01_exe_SOURCES = init.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c


dist_rtems_tests_DATA = psxmsgq01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxmsgq01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmsgq01_exe_OBJECTS) $(psxmsgq01_exe_LDADD)
LINK_LIBS = $(psxmsgq01_exe_LDLIBS)

psxmsgq01.exe$(EXEEXT): $(psxmsgq01_exe_OBJECTS) $(psxmsgq01_exe_DEPENDENCIES)
	@rm -f psxmsgq01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
