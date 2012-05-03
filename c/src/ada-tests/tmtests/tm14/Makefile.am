include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm14
ada_tm14_SOURCES = tm14.adb config.h tmtest.adb tmtest.ads
ada_tm14_SOURCES += ../../support/init.c

ada_tm14$(EXEEXT): tm14.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
