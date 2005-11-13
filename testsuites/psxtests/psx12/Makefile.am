##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx12.exe
psx12_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx12.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx12_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx12_exe_OBJECTS) $(psx12_exe_LDADD)
LINK_LIBS = $(psx12_exe_LDLIBS)

psx12.exe$(EXEEXT): $(psx12_exe_OBJECTS) $(psx12_exe_DEPENDENCIES)
	@rm -f psx12.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
