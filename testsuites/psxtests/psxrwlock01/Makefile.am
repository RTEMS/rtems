##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxrwlock01.exe
psxrwlock01_exe_SOURCES = main.c test.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxrwlock01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxrwlock01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxrwlock01_exe_OBJECTS) $(psxrwlock01_exe_LDADD)
LINK_LIBS = $(psxrwlock01_exe_LDLIBS)

psxrwlock01.exe$(EXEEXT): $(psxrwlock01_exe_OBJECTS) \
    $(psxrwlock01_exe_DEPENDENCIES)
	@rm -f psxrwlock01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
