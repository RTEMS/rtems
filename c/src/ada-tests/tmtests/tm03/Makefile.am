## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm03
tm03_SOURCES = tm03.adb config.h tmtest.adb tmtest.ads
tm03_SOURCES += ../../support/init.c

tm03$(EXEEXT): tm03.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
