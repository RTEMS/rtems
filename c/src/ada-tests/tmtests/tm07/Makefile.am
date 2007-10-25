## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm07.exe
tm07_exe_SOURCES = tm07.adb config.h tmtest.adb tmtest.ads
tm07_exe_SOURCES += ../../support/init.c

tm07.exe: tm07.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
