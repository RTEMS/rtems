include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

# Prevents Ada from raising a constraint so that the error
# handling can be verified in the binding.
AM_ADAFLAGS += -gnatp

noinst_PROGRAMS = ada_sp09

ada_sp09_SOURCES = sp09.adb config.h sptest.adb sptest.ads
ada_sp09_SOURCES += ../../support/init.c

ada_sp09$(EXEEXT): sp09.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp09.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
