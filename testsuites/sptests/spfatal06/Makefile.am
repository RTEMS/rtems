##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal06.exe
spfatal06_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal06.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal06_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal06_exe_OBJECTS) $(spfatal06_exe_LDADD)
LINK_LIBS = $(spfatal06_exe_LDLIBS)

spfatal06.exe$(EXEEXT): $(spfatal06_exe_OBJECTS) $(spfatal06_exe_DEPENDENCIES)
	@rm -f spfatal06.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
