## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm02
tm02_SOURCES = tm02.adb config.h tmtest.adb tmtest.ads
tm02_SOURCES += ../../support/init.c

tm02$(EXEEXT): tm02.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
