##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp61
sp61_SOURCES = init.c

dist_rtems_tests_DATA = sp61.scn
dist_rtems_tests_DATA += sp61.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp61_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp61_OBJECTS) $(sp61_LDADD)
LINK_LIBS = $(sp61_LDLIBS)

sp61$(EXEEXT): $(sp61_OBJECTS) $(sp61_DEPENDENCIES)
	@rm -f sp61$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
