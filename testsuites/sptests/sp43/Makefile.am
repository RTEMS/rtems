##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp43.exe
sp43_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp43.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp43_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp43_exe_OBJECTS) $(sp43_exe_LDADD)
LINK_LIBS = $(sp43_exe_LDLIBS)

sp43.exe$(EXEEXT): $(sp43_exe_OBJECTS) $(sp43_exe_DEPENDENCIES)
	@rm -f sp43.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
