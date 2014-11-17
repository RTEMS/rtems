rtems_tests_PROGRAMS = pwdgrp02
pwdgrp02_SOURCES = init.c

dist_rtems_tests_DATA = pwdgrp02.scn pwdgrp02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(pwdgrp02_OBJECTS)
LINK_LIBS = $(pwdgrp02_LDLIBS)

pwdgrp02$(EXEEXT): $(pwdgrp02_OBJECTS) $(pwdgrp02_DEPENDENCIES)
	@rm -f pwdgrp02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
