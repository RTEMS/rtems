## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp08
sp08_SOURCES = sp08.adb config.h sptest.adb sptest.ads
sp08_SOURCES += ../../support/init.c

sp08$(EXEEXT): sp08.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp08.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
