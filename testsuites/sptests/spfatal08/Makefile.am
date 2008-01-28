##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal08.exe
spfatal08_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal08.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal08_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal08_exe_OBJECTS) $(spfatal08_exe_LDADD)
LINK_LIBS = $(spfatal08_exe_LDLIBS)

spfatal08.exe$(EXEEXT): $(spfatal08_exe_OBJECTS) $(spfatal08_exe_DEPENDENCIES)
	@rm -f spfatal08.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
