##
## $Id$
##

MANAGERS = io message semaphore

rtems_tests_PROGRAMS = tm11.exe
tm11_exe_SOURCES = task1.c system.h ../include/timesys.h

dist_rtems_tests_DATA = tm11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tm11_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm11_exe_OBJECTS) $(tm11_exe_LDADD)
LINK_LIBS = $(tm11_exe_LDLIBS)

tm11.exe$(EXEEXT): $(tm11_exe_OBJECTS) $(tm11_exe_DEPENDENCIES)
	@rm -f tm11.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
