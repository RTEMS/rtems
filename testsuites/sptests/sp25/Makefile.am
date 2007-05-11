##
## $Id$
##

MANAGERS = io semaphore clock region

rtems_tests_PROGRAMS = sp25.exe
sp25_exe_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp25.scn
dist_rtems_tests_DATA += sp25.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp25_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp25_exe_OBJECTS) $(sp25_exe_LDADD)
LINK_LIBS = $(sp25_exe_LDLIBS)

sp25.exe$(EXEEXT): $(sp25_exe_OBJECTS) $(sp25_exe_DEPENDENCIES)
	@rm -f sp25.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
