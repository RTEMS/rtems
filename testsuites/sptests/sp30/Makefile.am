##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp30.exe
sp30_exe_SOURCES = init.c resume.c task1.c system.h

dist_rtems_tests_DATA = sp30.scn
dist_rtems_tests_DATA += sp30.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp30_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp30_exe_OBJECTS) $(sp30_exe_LDADD)
LINK_LIBS = $(sp30_exe_LDLIBS)

sp30.exe$(EXEEXT): $(sp30_exe_OBJECTS) $(sp30_exe_DEPENDENCIES)
	@rm -f sp30.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
