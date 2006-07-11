##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxreaddir.exe
psxreaddir_exe_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxreaddir.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxreaddir_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxreaddir_exe_OBJECTS) $(psxreaddir_exe_LDADD)
LINK_LIBS = $(psxreaddir_exe_LDLIBS)

psxreaddir.exe$(EXEEXT): $(psxreaddir_exe_OBJECTS) $(psxreaddir_exe_DEPENDENCIES)
	@rm -f psxreaddir.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
