##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronmbox01
itronmbox01_SOURCES = init.c system.h

dist_rtems_tests_DATA = itronmbox01.scn
dist_rtems_tests_DATA += itronmbox01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronmbox01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronmbox01_OBJECTS) $(itronmbox01_LDADD)
LINK_LIBS = $(itronmbox01_LDLIBS)

itronmbox01$(EXEEXT): $(itronmbox01_OBJECTS) $(itronmbox01_DEPENDENCIES)
	@rm -f itronmbox01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
