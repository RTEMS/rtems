##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxrdwrv.exe
psxrdwrv_exe_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxrdwrv.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxrdwrv_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxrdwrv_exe_OBJECTS) $(psxrdwrv_exe_LDADD)
LINK_LIBS = $(psxrdwrv_exe_LDLIBS)

psxrdwrv.exe$(EXEEXT): $(psxrdwrv_exe_OBJECTS) $(psxrdwrv_exe_DEPENDENCIES)
	@rm -f psxrdwrv.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
