##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal03.exe
spfatal03_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h
dist_rtems_tests_DATA = spfatal03.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal03_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal03_exe_OBJECTS) $(spfatal03_exe_LDADD)
LINK_LIBS = $(spfatal03_exe_LDLIBS)

spfatal03.exe$(EXEEXT): $(spfatal03_exe_OBJECTS) $(spfatal03_exe_DEPENDENCIES)
	@rm -f spfatal03.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
