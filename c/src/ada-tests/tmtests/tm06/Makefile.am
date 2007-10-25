## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm06.exe
tm06_exe_SOURCES = tm06.adb config.h tmtest.adb tmtest.ads
tm06_exe_SOURCES += ../../support/init.c

tm06.exe: tm06.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
