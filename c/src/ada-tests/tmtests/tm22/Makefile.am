## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm22.exe
tm22_exe_SOURCES = tm22.adb config.h tmtest.adb tmtest.ads
tm22_exe_SOURCES += ../../support/init.c

tm22.exe: tm22.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
