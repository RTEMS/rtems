## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm21
tm21_SOURCES = tm21.adb config.h tmtest.adb tmtest.ads
tm21_SOURCES += ../../support/init.c

tm21$(EXEEXT): tm21.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
