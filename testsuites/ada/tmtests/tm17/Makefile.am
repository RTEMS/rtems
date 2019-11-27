include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm17
ada_tm17_SOURCES = tm17.adb config.h tmtest.adb tmtest.ads
ada_tm17_SOURCES += init.c

ada_tm17$(EXEEXT): tm17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

