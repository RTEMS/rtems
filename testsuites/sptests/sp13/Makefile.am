##
## $Id$
##

MANAGERS = io message semaphore clock

rtems_tests_PROGRAMS = sp13.exe
sp13_exe_SOURCES = init.c fillbuff.c putbuff.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = sp13.scn
dist_rtems_tests_DATA += sp13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp13_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp13_exe_OBJECTS) $(sp13_exe_LDADD)
LINK_LIBS = $(sp13_exe_LDLIBS)

sp13.exe$(EXEEXT): $(sp13_exe_OBJECTS) $(sp13_exe_DEPENDENCIES)
	@rm -f sp13.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
