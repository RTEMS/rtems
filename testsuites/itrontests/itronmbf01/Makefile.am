##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronmbf01
itronmbf01_SOURCES = init.c system.h

dist_rtems_tests_DATA = itronmbf01.scn
dist_rtems_tests_DATA += itronmbf01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronmbf01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronmbf01_OBJECTS) $(itronmbf01_LDADD)
LINK_LIBS = $(itronmbf01_LDLIBS)

itronmbf01$(EXEEXT): $(itronmbf01_OBJECTS) $(itronmbf01_DEPENDENCIES)
	@rm -f itronmbf01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
