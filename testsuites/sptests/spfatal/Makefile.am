##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spfatal
spfatal_SOURCES = init.c puterr.c fatal.c task1.c system.h

dist_rtems_tests_DATA = spfatal.scn
dist_rtems_tests_DATA += spfatal.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spfatal_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal_OBJECTS) $(spfatal_LDADD)
LINK_LIBS = $(spfatal_LDLIBS)

spfatal$(EXEEXT): $(spfatal_OBJECTS) $(spfatal_DEPENDENCIES)
	@rm -f spfatal$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
