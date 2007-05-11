##
## $Id$
##

MANAGERS = io dpmem semaphore

rtems_tests_PROGRAMS = tm28.exe
tm28_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm28.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm28_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm28_exe_OBJECTS) $(tm28_exe_LDADD)
LINK_LIBS = $(tm28_exe_LDLIBS)

tm28.exe$(EXEEXT): $(tm28_exe_OBJECTS) $(tm28_exe_DEPENDENCIES)
	@rm -f tm28.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
