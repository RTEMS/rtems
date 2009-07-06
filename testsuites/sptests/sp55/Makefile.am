##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp55
sp55_SOURCES = init.c

dist_rtems_tests_DATA = sp55.scn
dist_rtems_tests_DATA += sp55.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp55_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMER_SERVER

LINK_OBJS = $(sp55_OBJECTS) $(sp55_LDADD)
LINK_LIBS = $(sp55_LDLIBS)

sp55$(EXEEXT): $(sp55_OBJECTS) $(sp55_DEPENDENCIES)
	@rm -f sp55$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
