## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp09

sp09_SOURCES = sp09.adb config.h sptest.adb sptest.ads
sp09_SOURCES += ../../support/init.c

sp09$(EXEEXT): sp09.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp09.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
