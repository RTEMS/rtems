##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp58
sp58_SOURCES = init.c

dist_rtems_tests_DATA = sp58.scn
dist_rtems_tests_DATA += sp58.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp58_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp58_OBJECTS) $(sp58_LDADD)
LINK_LIBS = $(sp58_LDLIBS)

sp58$(EXEEXT): $(sp58_OBJECTS) $(sp58_DEPENDENCIES)
	@rm -f sp58$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
