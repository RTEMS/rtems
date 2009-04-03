##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxfatal01
psxfatal01_SOURCES = ../psxfatal_support/init.c ../psxfatal_support/system.h testcase.h

dist_rtems_tests_DATA = psxfatal01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxfatal01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfatal01_OBJECTS) $(psxfatal01_LDADD)
LINK_LIBS = $(psxfatal01_LDLIBS)

psxfatal01$(EXEEXT): $(psxfatal01_OBJECTS) $(psxfatal01_DEPENDENCIES)
	@rm -f psxfatal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
