## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp12
sp12_SOURCES = sp12.adb config.h sptest.adb sptest.ads
sp12_SOURCES += ../../support/init.c

sp12$(EXEEXT): sp12.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp12.scn

include $(top_srcdir)/../tests/automake/local.am
