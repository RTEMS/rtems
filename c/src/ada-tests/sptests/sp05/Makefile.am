## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp05
sp05_SOURCES = sp05.adb config.h sptest.adb sptest.ads
sp05_SOURCES += ../../support/init.c

sp05$(EXEEXT): sp05.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp05.scn

include $(top_srcdir)/../tests/automake/local.am
