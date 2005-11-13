##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx07.exe
psx07_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psx07.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx07_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx07_exe_OBJECTS) $(psx07_exe_LDADD)
LINK_LIBS = $(psx07_exe_LDLIBS)

psx07.exe$(EXEEXT): $(psx07_exe_OBJECTS) $(psx07_exe_DEPENDENCIES)
	@rm -f psx07.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
