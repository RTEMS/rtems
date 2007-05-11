##
## $Id$
##

MANAGERS = io event timer clock semaphore

rtems_tests_PROGRAMS = sp11.exe
sp11_exe_SOURCES = init.c task1.c task2.c timer.c system.h

dist_rtems_tests_DATA = sp11.scn
dist_rtems_tests_DATA += sp11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp11_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp11_exe_OBJECTS) $(sp11_exe_LDADD)
LINK_LIBS = $(sp11_exe_LDLIBS)

sp11.exe$(EXEEXT): $(sp11_exe_OBJECTS) $(sp11_exe_DEPENDENCIES)
	@rm -f sp11.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
