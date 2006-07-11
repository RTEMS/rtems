##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx11.exe
psx11_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx11.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx11_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx11_exe_OBJECTS) $(psx11_exe_LDADD)
LINK_LIBS = $(psx11_exe_LDLIBS)

psx11.exe$(EXEEXT): $(psx11_exe_OBJECTS) $(psx11_exe_DEPENDENCIES)
	@rm -f psx11.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
