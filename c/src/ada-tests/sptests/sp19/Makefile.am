## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

if EXPADA
noinst_PROGRAMS = sp19
endif

sp19_SOURCES = sp19.adb config.h sptest.ads

# FIXME: Missing sptest.adb

sp19$(EXEEXT): sp19.adb
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp19.scn

include $(top_srcdir)/../../../automake/local.am
