##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask01
itrontask01_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = itrontask01.scn
dist_rtems_tests_DATA += itrontask01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask01_OBJECTS) $(itrontask01_LDADD)
LINK_LIBS = $(itrontask01_LDLIBS)

itrontask01$(EXEEXT): $(itrontask01_OBJECTS) $(itrontask01_DEPENDENCIES)
	@rm -f itrontask01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
