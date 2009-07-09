##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp59
sp59_SOURCES = init.c

dist_rtems_tests_DATA = sp59.scn
dist_rtems_tests_DATA += sp59.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp59_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp59_OBJECTS) $(sp59_LDADD)
LINK_LIBS = $(sp59_LDLIBS)

sp59$(EXEEXT): $(sp59_OBJECTS) $(sp59_DEPENDENCIES)
	@rm -f sp59$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
