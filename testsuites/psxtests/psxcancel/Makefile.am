##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxcancel.exe
psxcancel_exe_SOURCES = init.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxcancel.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxcancel_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcancel_exe_OBJECTS) $(psxcancel_exe_LDADD)
LINK_LIBS = $(psxcancel_exe_LDLIBS)

psxcancel.exe$(EXEEXT): $(psxcancel_exe_OBJECTS) $(psxcancel_exe_DEPENDENCIES)
	@rm -f psxcancel.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
