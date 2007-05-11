##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp35.exe
sp35_exe_SOURCES = priinv.c

dist_rtems_tests_DATA = sp35.scn
dist_rtems_tests_DATA += sp35.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp35_exe_OBJECTS) $(sp35_exe_LDADD)
LINK_LIBS = $(sp35_exe_LDLIBS)

sp35.exe$(EXEEXT): $(sp35_exe_OBJECTS) $(sp35_exe_DEPENDENCIES)
	@rm -f sp35.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
