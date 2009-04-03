##
## $Id$
##

MANAGERS = io event timer clock semaphore

rtems_tests_PROGRAMS = sp11
sp11_SOURCES = init.c task1.c task2.c timer.c system.h

dist_rtems_tests_DATA = sp11.scn
dist_rtems_tests_DATA += sp11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp11_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp11_OBJECTS) $(sp11_LDADD)
LINK_LIBS = $(sp11_LDLIBS)

sp11$(EXEEXT): $(sp11_OBJECTS) $(sp11_DEPENDENCIES)
	@rm -f sp11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
