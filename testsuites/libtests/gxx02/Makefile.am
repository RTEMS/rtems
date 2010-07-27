##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = gxx02
gxx02_SOURCES = init.c

dist_rtems_tests_DATA = gxx02.scn
dist_rtems_tests_DATA += gxx02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/gxx01

LINK_OBJS = $(gxx02_OBJECTS) $(gxx02_LDADD)
LINK_LIBS = $(gxx02_LDLIBS)

gxx02$(EXEEXT): $(gxx02_OBJECTS) $(gxx02_DEPENDENCIES)
	@rm -f gxx02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
