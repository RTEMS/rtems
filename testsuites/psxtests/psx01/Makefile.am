##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx01.exe
psx01_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx01_exe_OBJECTS) $(psx01_exe_LDADD)
LINK_LIBS = $(psx01_exe_LDLIBS)

psx01.exe$(EXEEXT): $(psx01_exe_OBJECTS) $(psx01_exe_DEPENDENCIES)
	@rm -f psx01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
