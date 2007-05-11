##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp01.exe
sp01_exe_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp01.scn
dist_rtems_tests_DATA += sp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp01_exe_OBJECTS) $(sp01_exe_LDADD)
LINK_LIBS = $(sp01_exe_LDLIBS)

sp01.exe$(EXEEXT): $(sp01_exe_OBJECTS) $(sp01_exe_DEPENDENCIES)
	@rm -f sp01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
