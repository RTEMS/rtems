## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp08
sp08_SOURCES = sp08.adb config.h sptest.adb sptest.ads

sp08$(EXEEXT): sp08.adb
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp08.scn

include $(top_srcdir)/../../../automake/local.am
