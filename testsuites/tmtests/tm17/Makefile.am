##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tm17.exe
tm17_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm17_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm17_exe_OBJECTS) $(tm17_exe_LDADD)
LINK_LIBS = $(tm17_exe_LDLIBS)

tm17.exe$(EXEEXT): $(tm17_exe_OBJECTS) $(tm17_exe_DEPENDENCIES)
	@rm -f tm17.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
