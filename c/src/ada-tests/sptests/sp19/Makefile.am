## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I.

noinst_PROGRAMS = sp19

sp19_SOURCES = sp19.adb sptest.adb config.h sptest.ads
sp19_SOURCES += ../../support/init.c

CLEANFILES += sptest.adb

sp19$(EXEEXT): sp19.adb sptest.adb sptest.ads init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp19.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
