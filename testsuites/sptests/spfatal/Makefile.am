##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal.exe
spfatal_exe_SOURCES = init.c puterr.c fatal.c task1.c system.h

dist_rtems_tests_DATA = spfatal.scn
dist_rtems_tests_DATA += spfatal.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal_exe_OBJECTS) $(spfatal_exe_LDADD)
LINK_LIBS = $(spfatal_exe_LDLIBS)

spfatal.exe$(EXEEXT): $(spfatal_exe_OBJECTS) $(spfatal_exe_DEPENDENCIES)
	@rm -f spfatal.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
