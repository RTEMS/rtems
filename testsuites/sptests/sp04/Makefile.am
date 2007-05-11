##
## $Id$
##

MANAGERS = io semaphore clock extension

rtems_tests_PROGRAMS = sp04.exe
sp04_exe_SOURCES = init.c task1.c task2.c task3.c tswitch.c system.h

dist_rtems_tests_DATA = sp04.scn
dist_rtems_tests_DATA += sp04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp04_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp04_exe_OBJECTS) $(sp04_exe_LDADD)
LINK_LIBS = $(sp04_exe_LDLIBS)

sp04.exe$(EXEEXT): $(sp04_exe_OBJECTS) $(sp04_exe_DEPENDENCIES)
	@rm -f sp04.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
