## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm03.exe
tm03_exe_SOURCES = tm03.adb config.h tmtest.adb tmtest.ads
tm03_exe_SOURCES += ../../support/init.c

tm03.exe: tm03.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
