##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp08
sp08_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp08.scn
dist_rtems_tests_DATA += sp08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp08_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp08_OBJECTS) $(sp08_LDADD)
LINK_LIBS = $(sp08_LDLIBS)

sp08$(EXEEXT): $(sp08_OBJECTS) $(sp08_DEPENDENCIES)
	@rm -f sp08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
