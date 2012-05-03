include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm11
ada_tm11_SOURCES = tm11.adb config.h tmtest.adb tmtest.ads
ada_tm11_SOURCES += ../../support/init.c

ada_tm11$(EXEEXT): tm11.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
