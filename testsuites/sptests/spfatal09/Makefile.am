##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal09.exe
spfatal09_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal09.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal09_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal09_exe_OBJECTS) $(spfatal09_exe_LDADD)
LINK_LIBS = $(spfatal09_exe_LDLIBS)

spfatal09.exe$(EXEEXT): $(spfatal09_exe_OBJECTS) $(spfatal09_exe_DEPENDENCIES)
	@rm -f spfatal09.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
