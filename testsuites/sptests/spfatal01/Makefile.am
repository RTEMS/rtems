##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal01.exe
spfatal01_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal01_exe_OBJECTS) $(spfatal01_exe_LDADD)
LINK_LIBS = $(spfatal01_exe_LDLIBS)

spfatal01.exe$(EXEEXT): $(spfatal01_exe_OBJECTS) $(spfatal01_exe_DEPENDENCIES)
	@rm -f spfatal01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
