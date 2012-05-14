include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm22
ada_tm22_SOURCES = tm22.adb config.h tmtest.adb tmtest.ads
ada_tm22_SOURCES += ../../support/init.c

ada_tm22$(EXEEXT): tm22.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
