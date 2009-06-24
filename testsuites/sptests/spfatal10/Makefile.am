##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal10
spfatal10_SOURCES = init.c

dist_rtems_tests_DATA = spfatal10.scn
dist_rtems_tests_DATA += spfatal10.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal10_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMER_SERVER

LINK_OBJS = $(spfatal10_OBJECTS) $(spfatal10_LDADD)
LINK_LIBS = $(spfatal10_LDLIBS)

spfatal10$(EXEEXT): $(spfatal10_OBJECTS) $(spfatal10_DEPENDENCIES)
	@rm -f spfatal10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
