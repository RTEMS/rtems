##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp37.exe
sp37_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp37.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp37_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp37_exe_OBJECTS) $(sp37_exe_LDADD)
LINK_LIBS = $(sp37_exe_LDLIBS)

sp37.exe$(EXEEXT): $(sp37_exe_OBJECTS) $(sp37_exe_DEPENDENCIES)
	@rm -f sp37.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
