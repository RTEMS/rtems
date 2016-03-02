rtems_tests_PROGRAMS = tmtimer01
tmtimer01_SOURCES = init.c

dist_rtems_tests_DATA = tmtimer01.scn tmtimer01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmtimer01_OBJECTS)
LINK_LIBS = $(tmtimer01_LDLIBS)

tmtimer01$(EXEEXT): $(tmtimer01_OBJECTS) $(tmtimer01_DEPENDENCIES)
	@rm -f tmtimer01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
