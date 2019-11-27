include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp05
ada_sp05_SOURCES = sp05.adb config.h sptest.adb sptest.ads
ada_sp05_SOURCES += init.c

ada_sp05$(EXEEXT): sp05.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp05.scn

