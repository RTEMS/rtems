##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp36.exe
sp36_exe_SOURCES = strict_order_mut.c

dist_rtems_tests_DATA = sp36.scn
dist_rtems_tests_DATA += sp36.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp36_exe_OBJECTS) $(sp36_exe_LDADD)
LINK_LIBS = $(sp36_exe_LDLIBS)

sp36.exe$(EXEEXT): $(sp36_exe_OBJECTS) $(sp36_exe_DEPENDENCIES)
	@rm -f sp36.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
