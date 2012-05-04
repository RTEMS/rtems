include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp22
ada_sp22_SOURCES = sp22.adb config.h sptest.adb sptest.ads
ada_sp22_SOURCES += ../../support/init.c

ada_sp22$(EXEEXT): sp22.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp22.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
