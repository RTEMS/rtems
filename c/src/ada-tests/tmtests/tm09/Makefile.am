## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm09
tm09_SOURCES = tm09.adb config.h tmtest.adb tmtest.ads
tm09_SOURCES += ../../support/init.c

tm09$(EXEEXT): tm09.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
