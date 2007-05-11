##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = pppd.exe
pppd_exe_SOURCES = init.c pppdapp.c netconfig.h system.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

pppd_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

pppd_exe_LDLIBS = -lpppd

LINK_OBJS = $(pppd_exe_OBJECTS) $(pppd_exe_LDADD)
LINK_LIBS = $(pppd_exe_LDLIBS)

pppd.exe$(EXEEXT): $(pppd_exe_OBJECTS) $(pppd_exe_DEPENDENCIES)
	@rm -f pppd.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
