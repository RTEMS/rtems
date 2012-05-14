include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm19
ada_tm19_SOURCES = tm19.adb config.h tmtest.adb tmtest.ads
ada_tm19_SOURCES += ../../support/init.c

ada_tm19$(EXEEXT): tm19.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
