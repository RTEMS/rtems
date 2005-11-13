##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxmount.exe
psxmount_exe_SOURCES = main.c test.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxmount.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxmount_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmount_exe_OBJECTS) $(psxmount_exe_LDADD)
LINK_LIBS = $(psxmount_exe_LDLIBS)

psxmount.exe$(EXEEXT): $(psxmount_exe_OBJECTS) $(psxmount_exe_DEPENDENCIES)
	@rm -f psxmount.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
