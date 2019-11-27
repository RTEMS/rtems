include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp06
ada_sp06_SOURCES = sp06.adb config.h sptest.adb sptest.ads
ada_sp06_SOURCES += init.c

ada_sp06$(EXEEXT): sp06.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp06.scn

