## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm18
tm18_SOURCES = tm18.adb config.h tmtest.adb tmtest.ads
tm18_SOURCES += ../../support/init.c

tm18$(EXEEXT): tm18.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
