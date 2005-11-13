##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx02.exe
psx02_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx02_exe_OBJECTS) $(psx02_exe_LDADD)
LINK_LIBS = $(psx02_exe_LDLIBS)

psx02.exe$(EXEEXT): $(psx02_exe_OBJECTS) $(psx02_exe_DEPENDENCIES)
	@rm -f psx02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
