##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp57
sp57_SOURCES = init.c

dist_rtems_tests_DATA = sp57.scn
dist_rtems_tests_DATA += sp57.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp57_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMER_SERVER

LINK_OBJS = $(sp57_OBJECTS) $(sp57_LDADD)
LINK_LIBS = $(sp57_LDLIBS)

sp57$(EXEEXT): $(sp57_OBJECTS) $(sp57_DEPENDENCIES)
	@rm -f sp57$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
