## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp22
sp22_SOURCES = sp22.adb config.h sptest.adb sptest.ads
sp22_SOURCES += ../../support/init.c

sp22$(EXEEXT): sp22.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp22.scn

include $(top_srcdir)/../../../automake/local.am
