## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

node1$(EXEEXT): mptest-per_node_configuration.adb
	$(GNATCOMPILE) -margs -a $< -o $@

if EXPADA
noinst_PROGRAMS = node1
endif

node1_SOURCES = mptest-per_node_configuration.adb

EXTRA_DIST = base_mp.scn

include $(top_srcdir)/../../../automake/local.am
