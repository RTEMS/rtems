## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp23
sp23_SOURCES = sp23.adb config.h sptest.adb sptest.ads
sp23_SOURCES += ../../support/init.c

sp23$(EXEEXT): sp23.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp23.scn

include $(top_srcdir)/../../../automake/local.am
