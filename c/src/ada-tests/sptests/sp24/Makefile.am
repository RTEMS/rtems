## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp24
sp24_SOURCES = sp24.adb config.h sptest.adb sptest.ads
sp24_SOURCES += ../../support/init.c

sp24$(EXEEXT): sp24.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp24.scn

include $(top_srcdir)/../tests/automake/local.am
