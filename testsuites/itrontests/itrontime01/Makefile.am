##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontime01
itrontime01_SOURCES = init.c system.h

dist_rtems_tests_DATA = itrontime01.scn
dist_rtems_tests_DATA += itrontime01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontime01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontime01_OBJECTS) $(itrontime01_LDADD)
LINK_LIBS = $(itrontime01_LDLIBS)

itrontime01$(EXEEXT): $(itrontime01_OBJECTS) $(itrontime01_DEPENDENCIES)
	@rm -f itrontime01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
