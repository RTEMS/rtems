## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

mptest$(EXEEXT): mptest.adb
	$(GNATCOMPILE) -margs -a $< -o $@

if EXPADA
noinst_PROGRAMS = mptest
endif

mptest_SOURCES = mptest.adb mptest.ads

EXTRA_DIST = node1/base_mp.scn
EXTRA_DIST += node2/base_mp.scn

include $(top_srcdir)/../../../automake/local.am
