## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp17
sp17_SOURCES = sp17.adb config.h sptest.adb sptest.ads

sp17$(EXEEXT): sp17.adb
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp17.scn

include $(top_srcdir)/../../../automake/local.am
