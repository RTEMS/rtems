## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I.

noinst_PROGRAMS = sp19

sp19_SOURCES = sp19.adb sptest.adb config.h sptest.ads

sptest.adb: sptest.adp
	pwd=`pwd` && cd $(srcdir) && $(M4) < sptest.adp > $${pwd}/sptest.adb
CLEANFILES += sptest.adb

sp19$(EXEEXT): sp19.adb sptest.adb sptest.ads
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp19.scn

include $(top_srcdir)/../../../automake/local.am
