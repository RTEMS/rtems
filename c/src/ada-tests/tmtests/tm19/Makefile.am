## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm19
tm19_SOURCES = tm19.adb config.h tmtest.adb tmtest.ads
tm19_SOURCES += ../../support/init.c

tm19$(EXEEXT): tm19.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
