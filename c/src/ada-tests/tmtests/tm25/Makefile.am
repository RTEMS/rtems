## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm25.exe
tm25_exe_SOURCES = tm25.adb config.h tmtest.adb tmtest.ads
tm25_exe_SOURCES += ../../support/init.c

tm25.exe: tm25.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
