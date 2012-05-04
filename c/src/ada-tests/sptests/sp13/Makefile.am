include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp13
ada_sp13_SOURCES = sp13.adb config.h sptest.adb sptest.ads
ada_sp13_SOURCES += ../../support/init.c

ada_sp13$(EXEEXT): sp13.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp13.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
