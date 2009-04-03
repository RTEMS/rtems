##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxfatal02
psxfatal02_SOURCES = ../psxfatal_support/init.c ../psxfatal_support/system.h testcase.h

dist_rtems_tests_DATA = psxfatal02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxfatal02_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfatal02_OBJECTS) $(psxfatal02_LDADD)
LINK_LIBS = $(psxfatal02_LDLIBS)

psxfatal02$(EXEEXT): $(psxfatal02_OBJECTS) $(psxfatal02_DEPENDENCIES)
	@rm -f psxfatal02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
