include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

_SUBDIRS = node1 node2
#include $(top_srcdir)/ada.am

#mptest$(EXEEXT): mptest.adb
#	$(GNATCOMPILE) -margs -a $< -o $@

#if EXPADA
#noinst_PROGRAMS = mptest
#endif

#mptest_exe_SOURCES = mptest.adb mptest.ads



include $(top_srcdir)/../automake/subdirs.am
