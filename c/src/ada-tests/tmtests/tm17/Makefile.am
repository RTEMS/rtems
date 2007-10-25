## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm17.exe
tm17_exe_SOURCES = tm17.adb config.h tmtest.adb tmtest.ads
tm17_exe_SOURCES += ../../support/init.c

tm17.exe: tm17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
