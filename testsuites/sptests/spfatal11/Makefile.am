##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal11
spfatal11_SOURCES = init.c

dist_rtems_tests_DATA = spfatal11.scn
dist_rtems_tests_DATA += spfatal11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal11_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMER_SERVER

LINK_OBJS = $(spfatal11_OBJECTS) $(spfatal11_LDADD)
LINK_LIBS = $(spfatal11_LDLIBS)

spfatal11$(EXEEXT): $(spfatal11_OBJECTS) $(spfatal11_DEPENDENCIES)
	@rm -f spfatal11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
