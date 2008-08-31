##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tm07.exe
tm07_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm07_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm07_exe_OBJECTS) $(tm07_exe_LDADD)
LINK_LIBS = $(tm07_exe_LDLIBS)

tm07.exe$(EXEEXT): $(tm07_exe_OBJECTS) $(tm07_exe_DEPENDENCIES)
	@rm -f tm07.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
