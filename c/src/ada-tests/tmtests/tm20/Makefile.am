## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm20.exe
tm20_exe_SOURCES = tm20.adb config.h tmtest.adb tmtest.ads
tm20_exe_SOURCES += ../../support/init.c

tm20.exe: tm20.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
