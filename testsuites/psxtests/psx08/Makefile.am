##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx08.exe
psx08_exe_SOURCES = init.c task1.c task2.c task3.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx08.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx08_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx08_exe_OBJECTS) $(psx08_exe_LDADD)
LINK_LIBS = $(psx08_exe_LDLIBS)

psx08.exe$(EXEEXT): $(psx08_exe_OBJECTS) $(psx08_exe_DEPENDENCIES)
	@rm -f psx08.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
