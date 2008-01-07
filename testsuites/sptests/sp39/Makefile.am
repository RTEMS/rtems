##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp39.exe
sp39_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp39.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp39_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp39_exe_OBJECTS) $(sp39_exe_LDADD)
LINK_LIBS = $(sp39_exe_LDLIBS)

sp39.exe$(EXEEXT): $(sp39_exe_OBJECTS) $(sp39_exe_DEPENDENCIES)
	@rm -f sp39.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
