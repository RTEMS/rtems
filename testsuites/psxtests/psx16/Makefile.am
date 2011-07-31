##
## $Id$
##

rtems_tests_PROGRAMS = psx16
psx16_SOURCES = init.c

dist_rtems_tests_DATA = psx16.scn
dist_rtems_tests_DATA += psx16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psx16_OBJECTS) $(psx16_LDADD)
LINK_LIBS = $(psx16_LDLIBS)

psx16$(EXEEXT): $(psx16_OBJECTS) $(psx16_DEPENDENCIES)
	@rm -f psx16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
