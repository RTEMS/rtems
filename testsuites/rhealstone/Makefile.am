#
# Real-time Benchmarking
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

rhealstones =
rhealstone_screens =
rhealstone_docs =

support_includes = -I$(top_srcdir)/../support/include

if TEST_rhdeadlockbrk
rhealstones += rhdeadlockbrk
rhealstone_docs += rhdeadlockbrk/rhdeadlockbrk.adoc
rhdeadlockbrk_SOURCES = rhdeadlockbrk/deadlockbrk.c \
	../tmtests/include/timesys.h
rhdeadlockbrk_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhdeadlockbrk) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_rhilatency
rhealstones += rhilatency
rhealstone_docs += rhilatency/rhilatency.adoc
rhilatency_SOURCES = rhilatency/ilatency.c \
	../tmtests/include/timesys.h
rhilatency_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhilatency) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_rhmlatency
rhealstones += rhmlatency
rhealstone_docs += rhmlatency/rhmlatency.adoc
rhmlatency_SOURCES = rhmlatency/mlatency.c \
	../tmtests/include/timesys.h
rhmlatency_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhmlatency) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_rhsemshuffle
rhealstones += rhsemshuffle
rhealstone_docs += rhsemshuffle/rhsemshuffle.adoc
rhsemshuffle_SOURCES = rhsemshuffle/semshuffle.c \
	../tmtests/include/timesys.h
rhsemshuffle_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhsemshuffle) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_rhtaskpreempt
rhealstones += rhtaskpreempt
rhealstone_docs += rhtaskpreempt/rhtaskpreempt.adoc
rhtaskpreempt_SOURCES = rhtaskpreempt/taskpreempt.c \
	../tmtests/include/timesys.h
rhtaskpreempt_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhtaskpreempt) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_rhtaskswitch
rhealstones += rhtaskswitch
rhealstone_docs += rhtaskswitch/rhtaskswitch.adoc
rhtaskswitch_SOURCES = rhtaskswitch/taskswitch.c \
	../tmtests/include/timesys.h
rhtaskswitch_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rhtaskswitch) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

noinst_PROGRAMS = $(rhealstones)
