## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm29
tm29_SOURCES = tm29.adb config.h tmtest.adb tmtest.ads
tm29_SOURCES += ../../support/init.c

tm29$(EXEEXT): tm29.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
