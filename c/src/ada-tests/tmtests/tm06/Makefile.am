## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm06
tm06_SOURCES = tm06.adb config.h tmtest.adb tmtest.ads
tm06_SOURCES += ../../support/init.c

tm06$(EXEEXT): tm06.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
