## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp16
sp16_SOURCES = sp16.adb config.h sptest.adb sptest.ads
sp16_SOURCES += ../../support/init.c

sp16$(EXEEXT): sp16.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp16.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
