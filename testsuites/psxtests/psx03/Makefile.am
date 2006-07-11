##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx03.exe
psx03_exe_SOURCES = init.c task.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psx03.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx03_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx03_exe_OBJECTS) $(psx03_exe_LDADD)
LINK_LIBS = $(psx03_exe_LDLIBS)

psx03.exe$(EXEEXT): $(psx03_exe_OBJECTS) $(psx03_exe_DEPENDENCIES)
	@rm -f psx03.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
