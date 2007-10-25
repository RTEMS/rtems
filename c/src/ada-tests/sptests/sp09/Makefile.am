## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

# Prevents Ada from raising a constraint so that the error
# handling can be verified in the binding.
AM_ADAFLAGS += -gnatp

noinst_PROGRAMS = sp09.exe

sp09_exe_SOURCES = sp09.adb config.h sptest.adb sptest.ads
sp09_exe_SOURCES += ../../support/init.c

sp09.exe: sp09.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp09.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
