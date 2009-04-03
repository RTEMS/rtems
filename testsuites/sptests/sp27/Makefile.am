##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp27
sp27_SOURCES = init.c

dist_rtems_tests_DATA = sp27.scn
dist_rtems_tests_DATA += sp27.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp27_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(sp27_OBJECTS) $(sp27_LDADD)
LINK_LIBS = $(sp27_LDLIBS)

sp27$(EXEEXT): $(sp27_OBJECTS) $(sp27_DEPENDENCIES)
	@rm -f sp27$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
