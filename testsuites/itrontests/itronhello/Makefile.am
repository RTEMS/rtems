##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronhello.exe
itronhello_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = itronhello.scn
dist_rtems_tests_DATA += itronhello.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronhello_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronhello_exe_OBJECTS) $(itronhello_exe_LDADD)
LINK_LIBS = $(itronhello_exe_LDLIBS)

itronhello.exe$(EXEEXT): $(itronhello_exe_OBJECTS) $(itronhello_exe_DEPENDENCIES)
	@rm -f itronhello.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
