##
## $Id$
##

MANAGERS = io signal timer semaphore clock

rtems_tests_PROGRAMS = sp14.exe
sp14_exe_SOURCES = init.c asr.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp14.scn
dist_rtems_tests_DATA += sp14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp14_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp14_exe_OBJECTS) $(sp14_exe_LDADD)
LINK_LIBS = $(sp14_exe_LDLIBS)

sp14.exe$(EXEEXT): $(sp14_exe_OBJECTS) $(sp14_exe_DEPENDENCIES)
	@rm -f sp14.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
