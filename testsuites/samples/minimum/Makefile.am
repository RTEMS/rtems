##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = minimum.exe
minimum_exe_SOURCES = init.c

dist_rtems_tests_DATA = minimum.scn
dist_rtems_tests_DATA += minimum.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

minimum_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

# 	This test should not be executed

LINK_OBJS = $(minimum_exe_OBJECTS) $(minimum_exe_LDADD)
LINK_LIBS = $(minimum_exe_LDLIBS)

minimum.exe$(EXEEXT): $(minimum_exe_OBJECTS) $(minimum_exe_DEPENDENCIES)
	@rm -f minimum.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
