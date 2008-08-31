##
## $Id$
##

MANAGERS = io dpmem message partition region semaphore rate_monotonic timer

rtems_tests_PROGRAMS = tm21.exe
tm21_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm21_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm21_exe_OBJECTS) $(tm21_exe_LDADD)
LINK_LIBS = $(tm21_exe_LDLIBS)

tm21.exe$(EXEEXT): $(tm21_exe_OBJECTS) $(tm21_exe_DEPENDENCIES)
	@rm -f tm21.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
