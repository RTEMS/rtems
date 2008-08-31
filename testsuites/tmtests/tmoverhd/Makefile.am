##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tmoverhd.exe
tmoverhd_exe_SOURCES = testtask.c empty.c system.h dumrtems.h \
    ../include/timesys.h ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tmoverhd.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tmoverhd_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmoverhd_exe_OBJECTS) $(tmoverhd_exe_LDADD)
LINK_LIBS = $(tmoverhd_exe_LDLIBS)

tmoverhd.exe$(EXEEXT): $(tmoverhd_exe_OBJECTS) $(tmoverhd_exe_DEPENDENCIES)
	@rm -f tmoverhd.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
