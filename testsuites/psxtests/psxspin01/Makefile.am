##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxspin01.exe
psxspin01_exe_SOURCES = main.c test.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxspin01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxspin01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxspin01_exe_OBJECTS) $(psxspin01_exe_LDADD)
LINK_LIBS = $(psxspin01_exe_LDLIBS)

psxspin01.exe$(EXEEXT): $(psxspin01_exe_OBJECTS) \
    $(psxspin01_exe_DEPENDENCIES)
	@rm -f psxspin01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
