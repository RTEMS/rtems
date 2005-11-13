##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx05.exe
psx05_exe_SOURCES = init.c task.c task2.c task3.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx05.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx05_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx05_exe_OBJECTS) $(psx05_exe_LDADD)
LINK_LIBS = $(psx05_exe_LDLIBS)

psx05.exe$(EXEEXT): $(psx05_exe_OBJECTS) $(psx05_exe_DEPENDENCIES)
	@rm -f psx05.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
