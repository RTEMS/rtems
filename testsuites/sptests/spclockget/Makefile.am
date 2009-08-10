##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spclockget
spclockget_SOURCES = init.c

dist_rtems_tests_DATA = spclockget.scn
dist_rtems_tests_DATA += spclockget.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spclockget_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spclockget_OBJECTS) $(spclockget_LDADD)
LINK_LIBS = $(spclockget_LDLIBS)

spclockget$(EXEEXT): $(spclockget_OBJECTS) $(spclockget_DEPENDENCIES)
	@rm -f spclockget$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
