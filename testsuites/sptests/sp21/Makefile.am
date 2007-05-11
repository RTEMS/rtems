##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp21.exe
sp21_exe_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp21.scn
dist_rtems_tests_DATA += sp21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp21_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp21_exe_OBJECTS) $(sp21_exe_LDADD)
LINK_LIBS = $(sp21_exe_LDLIBS)

sp21.exe$(EXEEXT): $(sp21_exe_OBJECTS) $(sp21_exe_DEPENDENCIES)
	@rm -f sp21.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
