##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = tmitronsem01
tmitronsem01_SOURCES = init.c system.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tmitronsem01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

tmitronsem01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmitronsem01_OBJECTS) $(tmitronsem01_LDADD)
LINK_LIBS = $(tmitronsem01_LDLIBS)

tmitronsem01$(EXEEXT): $(tmitronsem01_OBJECTS) $(tmitronsem01_DEPENDENCIES)
	@rm -f tmitronsem01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
