##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = pppd
pppd_SOURCES = init.c pppdapp.c netconfig.h system.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

pppd_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

pppd_LDLIBS = -lpppd

LINK_OBJS = $(pppd_OBJECTS) $(pppd_LDADD)
LINK_LIBS = $(pppd_LDLIBS)

pppd$(EXEEXT): $(pppd_OBJECTS) $(pppd_DEPENDENCIES)
	@rm -f pppd$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
