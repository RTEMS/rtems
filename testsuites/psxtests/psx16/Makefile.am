##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psx16.exe
psx16_exe_SOURCES = init.c

dist_rtems_tests_DATA = psx16.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psx16_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx16_exe_OBJECTS) $(psx16_exe_LDADD)
LINK_LIBS = $(psx16_exe_LDLIBS)

psx16.exe$(EXEEXT): $(psx16_exe_OBJECTS) $(psx16_exe_DEPENDENCIES)
	@rm -f psx16.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
