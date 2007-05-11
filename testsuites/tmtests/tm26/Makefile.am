##
## $Id$
##

MANAGERS = io semaphore semaphore

rtems_tests_PROGRAMS = tm26.exe
tm26_exe_SOURCES = task1.c system.h fptest.h ../include/timesys.h ../include/timesys.h

dist_rtems_tests_DATA = tm26.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm26_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm26_exe_OBJECTS) $(tm26_exe_LDADD)
LINK_LIBS = $(tm26_exe_LDLIBS)

tm26.exe$(EXEEXT): $(tm26_exe_OBJECTS) $(tm26_exe_DEPENDENCIES)
	@rm -f tm26.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
