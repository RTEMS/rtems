## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp02
sp02_SOURCES = sp02.adb config.h sptest.adb sptest.ads
sp02_SOURCES += ../../support/init.c

sp02$(EXEEXT): sp02.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp02.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
