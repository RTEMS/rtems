## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm17
tm17_SOURCES = tm17.adb config.h tmtest.adb tmtest.ads
tm17_SOURCES += ../../support/init.c

tm17$(EXEEXT): tm17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
