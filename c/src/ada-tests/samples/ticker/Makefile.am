## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

ticker$(EXEEXT): ticker.adb
	$(GNATCOMPILE) -margs -a $< -o $@

noinst_PROGRAMS = ticker

ticker_SOURCES = ticker.adb \
config.h sptest.adb sptest.ads

EXTRA_DIST = ticker.scn

include $(top_srcdir)/../../../automake/local.am
