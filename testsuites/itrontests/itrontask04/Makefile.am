##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask04
itrontask04_SOURCES = init.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = itrontask04.scn
dist_rtems_tests_DATA += itrontask04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask04_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask04_OBJECTS) $(itrontask04_LDADD)
LINK_LIBS = $(itrontask04_LDLIBS)

itrontask04$(EXEEXT): $(itrontask04_OBJECTS) $(itrontask04_DEPENDENCIES)
	@rm -f itrontask04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
