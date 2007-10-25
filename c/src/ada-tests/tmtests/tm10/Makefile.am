## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm10.exe
tm10_exe_SOURCES = tm10.adb config.h tmtest.adb tmtest.ads
tm10_exe_SOURCES += ../../support/init.c

tm10.exe: tm10.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
