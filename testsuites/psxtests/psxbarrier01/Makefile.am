##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxbarrier01.exe
psxbarrier01_exe_SOURCES = main.c test.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxbarrier01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxbarrier01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxbarrier01_exe_OBJECTS) $(psxbarrier01_exe_LDADD)
LINK_LIBS = $(psxbarrier01_exe_LDLIBS)

psxbarrier01.exe$(EXEEXT): $(psxbarrier01_exe_OBJECTS) \
    $(psxbarrier01_exe_DEPENDENCIES)
	@rm -f psxbarrier01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
