## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp14
sp14_SOURCES = sp14.adb config.h sptest.adb sptest.ads
sp14_SOURCES += ../../support/init.c

sp14$(EXEEXT): sp14.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp14.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
