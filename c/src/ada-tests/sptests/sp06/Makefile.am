## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp06
sp06_SOURCES = sp06.adb config.h sptest.adb sptest.ads
sp06_SOURCES += ../../support/init.c

sp06$(EXEEXT): sp06.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp06.scn

include $(top_srcdir)/../../../automake/local.am
