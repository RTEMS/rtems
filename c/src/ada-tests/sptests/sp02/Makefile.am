## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp02
sp02_SOURCES = sp02.adb config.h sptest.adb sptest.ads

sp02$(EXEEXT): sp02.adb
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp02.scn

include $(top_srcdir)/../../../automake/local.am
