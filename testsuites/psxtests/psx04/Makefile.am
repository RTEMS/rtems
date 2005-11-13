##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx04.exe
psx04_exe_SOURCES = init.c task1.c task2.c task3.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx04.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx04_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx04_exe_OBJECTS) $(psx04_exe_LDADD)
LINK_LIBS = $(psx04_exe_LDLIBS)

psx04.exe$(EXEEXT): $(psx04_exe_OBJECTS) $(psx04_exe_DEPENDENCIES)
	@rm -f psx04.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
