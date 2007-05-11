##
## $Id$
##

MANAGERS = io signal semaphore

rtems_tests_PROGRAMS = tm19.exe
tm19_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm19.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm19_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm19_exe_OBJECTS) $(tm19_exe_LDADD)
LINK_LIBS = $(tm19_exe_LDLIBS)

tm19.exe$(EXEEXT): $(tm19_exe_OBJECTS) $(tm19_exe_DEPENDENCIES)
	@rm -f tm19.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
