## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm15
tm15_SOURCES = tm15.adb config.h tmtest.adb tmtest.ads
tm15_SOURCES += ../../support/init.c

tm15$(EXEEXT): tm15.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
