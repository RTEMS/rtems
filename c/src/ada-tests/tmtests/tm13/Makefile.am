## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm13
tm13_SOURCES = tm13.adb config.h tmtest.adb tmtest.ads
tm13_SOURCES += ../../support/init.c

tm13$(EXEEXT): tm13.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
