## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm12
tm12_SOURCES = tm12.adb config.h tmtest.adb tmtest.ads
tm12_SOURCES += ../../support/init.c

tm12$(EXEEXT): tm12.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
