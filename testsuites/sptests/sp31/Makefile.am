##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp31.exe
sp31_exe_SOURCES = delay.c init.c prtime.c task1.c system.h

dist_rtems_tests_DATA = sp31.scn
dist_rtems_tests_DATA += sp31.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp31_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp31_exe_OBJECTS) $(sp31_exe_LDADD)
LINK_LIBS = $(sp31_exe_LDLIBS)

sp31.exe$(EXEEXT): $(sp31_exe_OBJECTS) $(sp31_exe_DEPENDENCIES)
	@rm -f sp31.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
