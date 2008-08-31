##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tmck.exe
tmck_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tmck.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tmck_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmck_exe_OBJECTS) $(tmck_exe_LDADD)
LINK_LIBS = $(tmck_exe_LDLIBS)

tmck.exe$(EXEEXT): $(tmck_exe_OBJECTS) $(tmck_exe_DEPENDENCIES)
	@rm -f tmck.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
