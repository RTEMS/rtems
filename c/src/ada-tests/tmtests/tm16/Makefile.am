## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm16
tm16_SOURCES = tm16.adb config.h tmtest.adb tmtest.ads
tm16_SOURCES += ../../support/init.c

tm16$(EXEEXT): tm16.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
