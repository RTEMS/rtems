##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tm27.exe
tm27_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm27.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm27_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm27_exe_OBJECTS) $(tm27_exe_LDADD)
LINK_LIBS = $(tm27_exe_LDLIBS)

tm27.exe$(EXEEXT): $(tm27_exe_OBJECTS) $(tm27_exe_DEPENDENCIES)
	@rm -f tm27.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
