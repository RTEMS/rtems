##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp42.exe
sp42_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp42.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp42_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp42_exe_OBJECTS) $(sp42_exe_LDADD)
LINK_LIBS = $(sp42_exe_LDLIBS)

sp42.exe$(EXEEXT): $(sp42_exe_OBJECTS) $(sp42_exe_DEPENDENCIES)
	@rm -f sp42.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
