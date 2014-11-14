rtems_tests_PROGRAMS = pwdgrp01
pwdgrp01_SOURCES = init.c

dist_rtems_tests_DATA = pwdgrp01.scn pwdgrp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(pwdgrp01_OBJECTS)
LINK_LIBS = $(pwdgrp01_LDLIBS)

pwdgrp01$(EXEEXT): $(pwdgrp01_OBJECTS) $(pwdgrp01_DEPENDENCIES)
	@rm -f pwdgrp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
