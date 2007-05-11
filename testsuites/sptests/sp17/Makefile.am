##
## $Id$
##

MANAGERS = io signal semaphore clock

rtems_tests_PROGRAMS = sp17.exe
sp17_exe_SOURCES = init.c asr.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp17.scn
dist_rtems_tests_DATA += sp17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp17_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp17_exe_OBJECTS) $(sp17_exe_LDADD)
LINK_LIBS = $(sp17_exe_LDLIBS)

sp17.exe$(EXEEXT): $(sp17_exe_OBJECTS) $(sp17_exe_DEPENDENCIES)
	@rm -f sp17.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
