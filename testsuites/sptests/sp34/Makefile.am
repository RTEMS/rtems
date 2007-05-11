##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp34.exe
sp34_exe_SOURCES = changepri.c

dist_rtems_tests_DATA = sp34.scn
dist_rtems_tests_DATA += sp34.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp34_exe_OBJECTS) $(sp34_exe_LDADD)
LINK_LIBS = $(sp34_exe_LDLIBS)

sp34.exe$(EXEEXT): $(sp34_exe_OBJECTS) $(sp34_exe_DEPENDENCIES)
	@rm -f sp34.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
