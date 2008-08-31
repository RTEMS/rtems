##
## $Id$
##

MANAGERS = io message semaphore

rtems_tests_PROGRAMS = tm12.exe
tm12_exe_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm12.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm12_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm12_exe_OBJECTS) $(tm12_exe_LDADD)
LINK_LIBS = $(tm12_exe_LDLIBS)

tm12.exe$(EXEEXT): $(tm12_exe_OBJECTS) $(tm12_exe_DEPENDENCIES)
	@rm -f tm12.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
