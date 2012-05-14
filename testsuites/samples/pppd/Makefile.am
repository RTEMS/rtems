
rtems_tests_PROGRAMS = pppd
pppd_SOURCES = init.c pppdapp.c netconfig.h system.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


pppd_LDLIBS = -lpppd

LINK_OBJS = $(pppd_OBJECTS)
LINK_LIBS = $(pppd_LDLIBS)

pppd$(EXEEXT): $(pppd_OBJECTS) $(pppd_DEPENDENCIES)
	@rm -f pppd$(EXEEXT)
	$(make-exe)

EXTRA_DIST =
EXTRA_DIST += ppp.conf
EXTRA_DIST += pppd.options

include $(top_srcdir)/../automake/local.am
