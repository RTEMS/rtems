MANAGERS = all

rtems_tests_PROGRAMS = rhilatency
rhilatency_SOURCES  = ilatency.c
rhilatency_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = rhilatency.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rhilatency_OBJECTS) $(rhilatency_ldaDD)
LINK_LIBS = $(rhilatency_LDLIBS)

rhilatency$(EXEEXT): $(rhilatency_OBJECTS) $(rhilatency_DEPENDENCIES)
	@rm -f rhilatency$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
