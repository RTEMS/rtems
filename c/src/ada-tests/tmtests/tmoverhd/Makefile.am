## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tmoverhd
tmoverhd_SOURCES = tmoverhd.adb config.h \
tmtest.adb tmtest.ads \
dummy_rtems.adb dummy_rtems.ads

tmoverhd$(EXEEXT): tmoverhd.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
