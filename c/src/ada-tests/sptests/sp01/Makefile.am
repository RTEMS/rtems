## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp01
sp01_SOURCES = sp01.adb config.h sptest.adb sptest.ads
sp01_SOURCES += ../../support/init.c

sp01$(EXEEXT): sp01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp01.scn

include $(top_srcdir)/../tests/automake/local.am
