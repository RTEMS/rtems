include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm02
ada_tm02_SOURCES = tm02.adb config.h tmtest.adb tmtest.ads
ada_tm02_SOURCES += ../../support/init.c

ada_tm02$(EXEEXT): tm02.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
