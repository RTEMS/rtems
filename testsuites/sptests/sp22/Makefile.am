##
## $Id$
##

MANAGERS = io timer semaphore clock

rtems_tests_PROGRAMS = sp22.exe
sp22_exe_SOURCES = init.c prtime.c delay.c task1.c system.h

dist_rtems_tests_DATA = sp22.scn
dist_rtems_tests_DATA += sp22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp22_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp22_exe_OBJECTS) $(sp22_exe_LDADD)
LINK_LIBS = $(sp22_exe_LDLIBS)

sp22.exe$(EXEEXT): $(sp22_exe_OBJECTS) $(sp22_exe_DEPENDENCIES)
	@rm -f sp22.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
