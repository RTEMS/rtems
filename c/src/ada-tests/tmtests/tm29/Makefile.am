## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm29.exe
tm29_exe_SOURCES = tm29.adb config.h tmtest.adb tmtest.ads
tm29_exe_SOURCES += ../../support/init.c

tm29.exe: tm29.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
