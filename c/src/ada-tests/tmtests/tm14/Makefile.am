## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm14
tm14_SOURCES = tm14.adb config.h tmtest.adb tmtest.ads
tm14_SOURCES += ../../support/init.c

tm14$(EXEEXT): tm14.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
