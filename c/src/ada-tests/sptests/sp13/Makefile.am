## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp13
sp13_SOURCES = sp13.adb config.h sptest.adb sptest.ads
sp13_SOURCES += ../../support/init.c

sp13$(EXEEXT): sp13.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp13.scn

include $(top_srcdir)/../../../automake/local.am
