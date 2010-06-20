##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfifo08
spfifo08_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = spfifo08.scn
dist_rtems_tests_DATA += spfifo08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfifo08_OBJECTS) $(spfifo08_LDADD)
LINK_LIBS = $(spfifo08_LDLIBS)

spfifo08$(EXEEXT): $(spfifo08_OBJECTS) $(spfifo08_DEPENDENCIES)
	@rm -f spfifo08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
