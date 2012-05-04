include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm04
ada_tm04_SOURCES = tm04.adb config.h tmtest.adb tmtest.ads
ada_tm04_SOURCES += ../../support/init.c

ada_tm04$(EXEEXT): tm04.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
