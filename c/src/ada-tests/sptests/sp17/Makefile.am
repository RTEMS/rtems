## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp17
sp17_SOURCES = sp17.adb config.h sptest.adb sptest.ads
sp17_SOURCES += ../../support/init.c

sp17$(EXEEXT): sp17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp17.scn

include $(top_srcdir)/../tests/automake/local.am
