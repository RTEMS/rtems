## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp20
sp20_SOURCES = sp20.adb config.h sptest.adb sptest.ads
sp20_SOURCES += ../../support/init.c

sp20$(EXEEXT): sp20.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp20.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
