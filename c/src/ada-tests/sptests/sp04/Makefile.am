include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp04
ada_sp04_SOURCES = sp04.adb config.h sptest.adb sptest.ads
ada_sp04_SOURCES += ../../support/init.c

ada_sp04$(EXEEXT): sp04.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp04.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
