## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm11
tm11_SOURCES = tm11.adb config.h tmtest.adb tmtest.ads
tm11_SOURCES += ../../support/init.c

tm11$(EXEEXT): tm11.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
