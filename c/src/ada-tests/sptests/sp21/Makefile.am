## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp21

sp21_SOURCES = sp21.adb config.h sptest.adb sptest.ads
sp21_SOURCES += ../../support/init.c

sp21$(EXEEXT): sp21.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp21.scn

include $(top_srcdir)/../../../automake/local.am
