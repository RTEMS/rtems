## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm07
tm07_SOURCES = tm07.adb config.h tmtest.adb tmtest.ads
tm07_SOURCES += ../../support/init.c

tm07$(EXEEXT): tm07.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
