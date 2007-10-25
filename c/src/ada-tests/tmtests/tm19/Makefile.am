## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm19.exe
tm19_exe_SOURCES = tm19.adb config.h tmtest.adb tmtest.ads
tm19_exe_SOURCES += ../../support/init.c

tm19.exe: tm19.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
