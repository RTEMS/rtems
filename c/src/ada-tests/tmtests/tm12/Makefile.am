## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm12.exe
tm12_exe_SOURCES = tm12.adb config.h tmtest.adb tmtest.ads
tm12_exe_SOURCES += ../../support/init.c

tm12.exe: tm12.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
