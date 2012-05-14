include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

SUBDIRS = node1 node2
#include $(top_srcdir)/ada.am

#mptest$(EXEEXT): mptest.adb
#	$(GNATCOMPILE) -margs -a $< -o $@

#if EXPADA
#noinst_PROGRAMS = mptest
#endif

#mptest_exe_SOURCES = mptest.adb mptest.ads

#EXTRA_DIST += node1/mptest-per_node_configuration.adb

#EXTRA_DIST += node2/mptest-per_node_configuration.adb

include $(top_srcdir)/../../../testsuites/automake/subdirs.am
include $(top_srcdir)/../../../testsuites/automake/local.am
