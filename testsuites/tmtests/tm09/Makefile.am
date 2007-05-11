##
## $Id$
##

MANAGERS = io message semaphore

rtems_tests_PROGRAMS = tm09.exe
tm09_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm09_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm09_exe_OBJECTS) $(tm09_exe_LDADD)
LINK_LIBS = $(tm09_exe_LDLIBS)

tm09.exe$(EXEEXT): $(tm09_exe_OBJECTS) $(tm09_exe_DEPENDENCIES)
	@rm -f tm09.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
