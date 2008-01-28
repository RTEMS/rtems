##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal04.exe
spfatal04_exe_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal04.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal04_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal04_exe_OBJECTS) $(spfatal04_exe_LDADD)
LINK_LIBS = $(spfatal04_exe_LDLIBS)

spfatal04.exe$(EXEEXT): $(spfatal04_exe_OBJECTS) $(spfatal04_exe_DEPENDENCIES)
	@rm -f spfatal04.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
