##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx14.exe
psx14_exe_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx14.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx14_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx14_exe_OBJECTS) $(psx14_exe_LDADD)
LINK_LIBS = $(psx14_exe_LDLIBS)

psx14.exe$(EXEEXT): $(psx14_exe_OBJECTS) $(psx14_exe_DEPENDENCIES)
	@rm -f psx14.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
