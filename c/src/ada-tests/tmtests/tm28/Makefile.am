## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm28
tm28_SOURCES = tm28.adb config.h tmtest.adb tmtest.ads
tm28_SOURCES += ../../support/init.c

tm28$(EXEEXT): tm28.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
