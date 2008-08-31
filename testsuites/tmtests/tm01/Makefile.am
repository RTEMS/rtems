##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tm01.exe
tm01_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm01_exe_OBJECTS) $(tm01_exe_LDADD)
LINK_LIBS = $(tm01_exe_LDLIBS)

tm01.exe$(EXEEXT): $(tm01_exe_OBJECTS) $(tm01_exe_DEPENDENCIES)
	@rm -f tm01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
