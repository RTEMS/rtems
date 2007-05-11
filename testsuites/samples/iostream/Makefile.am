##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = iostream.exe
iostream_exe_SOURCES = init.cc system.h

dist_rtems_tests_DATA = iostream.scn
dist_rtems_tests_DATA += iostream.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

iostream_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(iostream_exe_OBJECTS) $(iostream_exe_LDADD)
LINK_LIBS = $(iostream_exe_LDLIBS)

iostream.exe$(EXEEXT): $(iostream_exe_OBJECTS) $(iostream_exe_DEPENDENCIES)
	@rm -f iostream.exe$(EXEEXT)
	$(make-cxx-exe)

include $(top_srcdir)/../automake/local.am
