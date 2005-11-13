##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx10.exe
psx10_exe_SOURCES = init.c task.c task2.c task3.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx10.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx10_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx10_exe_OBJECTS) $(psx10_exe_LDADD)
LINK_LIBS = $(psx10_exe_LDLIBS)

psx10.exe$(EXEEXT): $(psx10_exe_OBJECTS) $(psx10_exe_DEPENDENCIES)
	@rm -f psx10.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
