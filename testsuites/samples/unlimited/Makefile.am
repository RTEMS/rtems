##
##  $Id$
##

MANAGERS = io event semaphore

rtems_tests_PROGRAMS = unlimited.exe
unlimited_exe_SOURCES = init.c test1.c test2.c test3.c system.h

dist_rtems_tests_DATA = unlimited.scn
dist_rtems_tests_DATA += unlimited.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

unlimited_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(unlimited_exe_OBJECTS) $(unlimited_exe_LDADD)
LINK_LIBS = $(unlimited_exe_LDLIBS)

unlimited.exe$(EXEEXT): $(unlimited_exe_OBJECTS) $(unlimited_exe_DEPENDENCIES)
	@rm -f unlimited.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
