##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = putenvtest
putenvtest_SOURCES = init.c

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

putenvtest_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(putenvtest_OBJECTS) $(putenvtest_LDADD)
LINK_LIBS = $(putenvtest_LDLIBS)

putenvtest$(EXEEXT): $(putenvtest_OBJECTS) $(putenvtest_DEPENDENCIES)
	@rm -f putenvtest$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
