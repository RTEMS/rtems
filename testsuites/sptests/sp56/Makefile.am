##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp56
sp56_SOURCES = init.c

dist_rtems_tests_DATA = sp56.scn
dist_rtems_tests_DATA += sp56.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp56_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMER_SERVER

LINK_OBJS = $(sp56_OBJECTS) $(sp56_LDADD)
LINK_LIBS = $(sp56_LDLIBS)

sp56$(EXEEXT): $(sp56_OBJECTS) $(sp56_DEPENDENCIES)
	@rm -f sp56$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
