## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm24
tm24_SOURCES = tm24.adb config.h tmtest.adb tmtest.ads
tm24_SOURCES += ../../support/init.c

tm24$(EXEEXT): tm24.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
