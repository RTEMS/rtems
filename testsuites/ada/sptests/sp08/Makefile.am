include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp08
ada_sp08_SOURCES = sp08.adb config.h sptest.adb sptest.ads
ada_sp08_SOURCES += init.c

ada_sp08$(EXEEXT): sp08.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp08.scn

