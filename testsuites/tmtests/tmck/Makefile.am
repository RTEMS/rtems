##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = tmck
tmck_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tmck.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tmck_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmck_OBJECTS) $(tmck_LDADD)
LINK_LIBS = $(tmck_LDLIBS)

tmck$(EXEEXT): $(tmck_OBJECTS) $(tmck_DEPENDENCIES)
	@rm -f tmck$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
