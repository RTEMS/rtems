## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm05
tm05_SOURCES = tm05.adb config.h tmtest.adb tmtest.ads
tm05_SOURCES += ../../support/init.c

tm05$(EXEEXT): tm05.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
