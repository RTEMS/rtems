## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp15
sp15_SOURCES = sp15.adb config.h sptest.adb sptest.ads

sp15$(EXEEXT): sp15.adb
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp15.scn

include $(top_srcdir)/../../../automake/local.am
