include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp20
ada_sp20_SOURCES = sp20.adb config.h sptest.adb sptest.ads
ada_sp20_SOURCES += ../../support/init.c

ada_sp20$(EXEEXT): sp20.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp20.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
