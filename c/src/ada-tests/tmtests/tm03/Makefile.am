include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm03
ada_tm03_SOURCES = tm03.adb config.h tmtest.adb tmtest.ads
ada_tm03_SOURCES += ../../support/init.c

ada_tm03$(EXEEXT): tm03.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
