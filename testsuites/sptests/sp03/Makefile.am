##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp03.exe
sp03_exe_SOURCES = init.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp03.scn
dist_rtems_tests_DATA += sp03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp03_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp03_exe_OBJECTS) $(sp03_exe_LDADD)
LINK_LIBS = $(sp03_exe_LDLIBS)

sp03.exe$(EXEEXT): $(sp03_exe_OBJECTS) $(sp03_exe_DEPENDENCIES)
	@rm -f sp03.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
