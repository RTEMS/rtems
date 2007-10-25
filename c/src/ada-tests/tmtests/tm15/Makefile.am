## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm15.exe
tm15_exe_SOURCES = tm15.adb config.h tmtest.adb tmtest.ads
tm15_exe_SOURCES += ../../support/init.c

tm15.exe: tm15.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
