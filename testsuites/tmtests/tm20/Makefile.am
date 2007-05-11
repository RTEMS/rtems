##
## $Id$
##

MANAGERS = io partition region semaphore

rtems_tests_PROGRAMS = tm20.exe
tm20_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm20.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm20_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm20_exe_OBJECTS) $(tm20_exe_LDADD)
LINK_LIBS = $(tm20_exe_LDLIBS)

tm20.exe$(EXEEXT): $(tm20_exe_OBJECTS) $(tm20_exe_DEPENDENCIES)
	@rm -f tm20.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
