## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp04
sp04_SOURCES = sp04.adb config.h sptest.adb sptest.ads
sp04_SOURCES += ../../support/init.c

sp04$(EXEEXT): sp04.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp04.scn

include $(top_srcdir)/../../../automake/local.am
