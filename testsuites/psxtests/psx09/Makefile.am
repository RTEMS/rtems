##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx09.exe
psx09_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx09.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx09_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx09_exe_OBJECTS) $(psx09_exe_LDADD)
LINK_LIBS = $(psx09_exe_LDLIBS)

psx09.exe$(EXEEXT): $(psx09_exe_OBJECTS) $(psx09_exe_DEPENDENCIES)
	@rm -f psx09.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
