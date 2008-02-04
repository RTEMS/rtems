##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxkey01.exe
psxkey01_exe_SOURCES = init.c task.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c


dist_rtems_tests_DATA = psxkey01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxkey01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey01_exe_OBJECTS) $(psxkey01_exe_LDADD)
LINK_LIBS = $(psxkey01_exe_LDLIBS)

psxkey01.exe$(EXEEXT): $(psxkey01_exe_OBJECTS) $(psxkey01_exe_DEPENDENCIES)
	@rm -f psxkey01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
