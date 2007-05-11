##
## $Id$
##

MANAGERS = io partition semaphore clock

rtems_tests_PROGRAMS = sp15.exe
sp15_exe_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp15.scn
dist_rtems_tests_DATA += sp15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp15_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp15_exe_OBJECTS) $(sp15_exe_LDADD)
LINK_LIBS = $(sp15_exe_LDLIBS)

sp15.exe$(EXEEXT): $(sp15_exe_OBJECTS) $(sp15_exe_DEPENDENCIES)
	@rm -f sp15.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
