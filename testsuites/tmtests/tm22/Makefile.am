##
## $Id$
##

MANAGERS = io message semaphore

rtems_tests_PROGRAMS = tm22.exe
tm22_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm22_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm22_exe_OBJECTS) $(tm22_exe_LDADD)
LINK_LIBS = $(tm22_exe_LDLIBS)

tm22.exe$(EXEEXT): $(tm22_exe_OBJECTS) $(tm22_exe_DEPENDENCIES)
	@rm -f tm22.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
