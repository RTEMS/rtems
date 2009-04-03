##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask02
itrontask02_SOURCES = init.c dormant.c system.h

dist_rtems_tests_DATA = itrontask02.scn
dist_rtems_tests_DATA += itrontask02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask02_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask02_OBJECTS) $(itrontask02_LDADD)
LINK_LIBS = $(itrontask02_LDLIBS)

itrontask02$(EXEEXT): $(itrontask02_OBJECTS) $(itrontask02_DEPENDENCIES)
	@rm -f itrontask02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
