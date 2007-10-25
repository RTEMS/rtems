## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm04.exe
tm04_exe_SOURCES = tm04.adb config.h tmtest.adb tmtest.ads
tm04_exe_SOURCES += ../../support/init.c

tm04.exe: tm04.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
