##
## $Id$
##

MANAGERS = io event timer semaphore clock

rtems_tests_PROGRAMS = sp24.exe
sp24_exe_SOURCES = init.c resume.c task1.c system.h

dist_rtems_tests_DATA = sp24.scn
dist_rtems_tests_DATA += sp24.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp24_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp24_exe_OBJECTS) $(sp24_exe_LDADD)
LINK_LIBS = $(sp24_exe_LDLIBS)

sp24.exe$(EXEEXT): $(sp24_exe_OBJECTS) $(sp24_exe_DEPENDENCIES)
	@rm -f sp24.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
