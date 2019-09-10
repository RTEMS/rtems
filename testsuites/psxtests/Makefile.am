#
# POSIX Teststuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

psx_tests =
psx_screens =
psx_docs =
psx_lib =

support_includes = -I$(top_srcdir)/../support/include

if TEST_psx01
psx_tests += psx01
psx01_SOURCES = psx01/init.c psx01/task.c psx01/system.h \
	include/pmacros.h
psx01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psx02
psx_tests += psx02
psx_screens += psx02/psx02.scn
psx02_SOURCES = psx02/init.c psx02/task.c psx02/system.h \
	include/pmacros.h
psx02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx02) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psx03
psx_tests += psx03
psx_screens += psx03/psx03.scn
psx03_SOURCES = psx03/init.c psx03/task.c psx03/system.h \
	include/pmacros.h
psx03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx03) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psx04
psx_tests += psx04
psx_screens += psx04/psx04.scn
psx04_SOURCES = psx04/init.c psx04/task1.c psx04/task2.c psx04/task3.c \
	psx04/system.h include/pmacros.h
psx04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx04) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psx05
psx_tests += psx05
psx_screens += psx05/psx05.scn
psx05_SOURCES = psx05/init.c psx05/task.c psx05/task2.c psx05/task3.c \
	psx05/system.h include/pmacros.h
psx05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx05) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx06
psx_tests += psx06
psx_screens += psx06/psx06.scn
psx06_SOURCES = psx06/init.c psx06/task.c psx06/task2.c psx06/system.h \
	include/pmacros.h
psx06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx06) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx07
psx_tests += psx07
psx_screens += psx07/psx07.scn
psx_docs += psx07/psx07.doc
psx07_SOURCES = psx07/init.c psx07/task.c psx07/system.h \
	include/pmacros.h
psx07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx07) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx08
psx_tests += psx08
psx_screens += psx08/psx08.scn
psx08_SOURCES = psx08/init.c psx08/task1.c psx08/task2.c psx08/task3.c \
	psx08/system.h include/pmacros.h
psx08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx08) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psx09
psx_tests += psx09
psx_screens += psx09/psx09.scn
psx09_SOURCES = psx09/init.c psx09/system.h include/pmacros.h
psx09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx09) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psx10
psx_tests += psx10
psx_screens += psx10/psx10.scn
psx10_SOURCES = psx10/init.c psx10/task.c psx10/task2.c psx10/task3.c \
	psx10/system.h include/pmacros.h
psx10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx10) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx11
psx_tests += psx11
psx_screens += psx11/psx11.scn
psx11_SOURCES = psx11/init.c psx11/task.c psx11/system.h \
	include/pmacros.h
psx11_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx11) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psx12
psx_tests += psx12
psx_screens += psx12/psx12.scn
psx12_SOURCES = psx12/init.c include/pmacros.h
psx12_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx12) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psx13
psx_tests += psx13
psx_screens += psx13/psx13.scn
psx13_SOURCES = psx13/main.c psx13/test.c include/pmacros.h
psx13_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx13) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx14
psx_tests += psx14
psx_screens += psx14/psx14.scn
psx14_SOURCES = psx14/init.c psx14/system.h include/pmacros.h
psx14_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx14) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx15
psx_tests += psx15
psx_screens += psx15/psx15.scn
psx15_SOURCES = psx15/init.c include/pmacros.h
psx15_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx15) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psx16
psx_tests += psx16
psx_screens += psx16/psx16.scn
psx16_SOURCES = psx16/init.c include/pmacros.h
psx16_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psx16) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psxaio01
psx_tests += psxaio01
psx_screens += psxaio01/psxaio01.scn
psxaio01_SOURCES = psxaio01/init.c psxaio01/system.h include/pmacros.h
psxaio01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxaio01) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxaio02
psx_tests += psxaio02
psx_screens += psxaio02/psxaio02.scn
psxaio02_SOURCES = psxaio02/init.c psxaio02/system.h include/pmacros.h
psxaio02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxaio02) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxaio03
psx_tests += psxaio03
psx_screens += psxaio03/psxaio03.scn
psxaio03_SOURCES = psxaio03/init.c psxaio03/system.h include/pmacros.h
psxaio03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxaio03) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxalarm01
psx_tests += psxalarm01
psx_screens += psxalarm01/psxalarm01.scn
psx_docs += psxalarm01/psxalarm01.doc
psxalarm01_SOURCES = psxalarm01/init.c include/pmacros.h
psxalarm01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxalarm01) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxautoinit01
psx_tests += psxautoinit01
psx_screens += psxautoinit01/psxautoinit01.scn
psxautoinit01_SOURCES = psxautoinit01/init.c include/pmacros.h
psxautoinit01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxautoinit01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxautoinit02
psx_tests += psxautoinit02
psx_screens += psxautoinit02/psxautoinit02.scn
psx_docs += psxautoinit02/psxautoinit02.doc
psxautoinit02_SOURCES = psxautoinit02/init.c include/pmacros.h
psxautoinit02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxautoinit02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxbarrier01
psx_tests += psxbarrier01
psx_screens += psxbarrier01/psxbarrier01.scn
psxbarrier01_SOURCES = psxbarrier01/main.c psxbarrier01/test.c \
	include/pmacros.h
psxbarrier01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxbarrier01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxcancel
psx_tests += psxcancel
psx_screens += psxcancel/psxcancel.scn
psx_docs += psxcancel/psxcancel.doc
psxcancel_SOURCES = psxcancel/init.c include/pmacros.h
psxcancel_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcancel) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxcancel01
psx_tests += psxcancel01
psx_screens += psxcancel01/psxcancel01.scn
psx_docs += psxcancel01/psxcancel01.doc
psxcancel01_SOURCES = psxcancel01/init.c include/pmacros.h
psxcancel01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcancel01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxchroot01
psx_tests += psxchroot01
psx_screens += psxchroot01/psxchroot01.scn
psxchroot01_SOURCES = psxchroot01/main.c psxchroot01/test.c \
	include/pmacros.h
psxchroot01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxchroot01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psxclassic01
psx_tests += psxclassic01
psx_screens += psxclassic01/psxclassic01.scn
psx_docs += psxclassic01/psxclassic01.doc
psxclassic01_SOURCES = psxclassic01/init.c include/pmacros.h
psxclassic01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxclassic01) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxcleanup
psx_tests += psxcleanup
psx_screens += psxcleanup/psxcleanup.scn
psxcleanup_SOURCES = psxcleanup/psxcleanup.c psxcleanup/system.h \
	include/pmacros.h
psxcleanup_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcleanup) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxcleanup01
psx_tests += psxcleanup01
psx_screens += psxcleanup01/psxcleanup01.scn
psx_docs += psxcleanup01/psxcleanup01.doc
psxcleanup01_SOURCES = psxcleanup01/init.c include/pmacros.h
psxcleanup01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcleanup01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxcleanup02
psx_tests += psxcleanup02
psx_screens += psxcleanup02/psxcleanup02.scn
psx_docs += psxcleanup02/psxcleanup02.doc
psxcleanup02_SOURCES = psxcleanup02/init.c psxcleanup02/main.c
psxcleanup02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcleanup02) \
	$(support_includes)
endif

if TEST_psxclock
psx_tests += psxclock
psx_screens += psxclock/psxclock.scn
psx_docs += psxclock/psxclock.doc
psxclock_SOURCES = psxclock/init.c include/pmacros.h
psxclock_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxclock) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxclock01
psx_tests += psxclock01
psx_screens += psxclock01/psxclock01.scn
psx_docs += psxclock01/psxclock01.doc
psxclock01_SOURCES = psxclock01/init.c
psxclock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxclock01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxclockrealtime01
psx_tests += psxclockrealtime01
psx_screens += psxclockrealtime01/psxclockrealtime01.scn
psx_docs += psxclockrealtime01/psxclockrealtime01.doc
psxclockrealtime01_SOURCES = psxclockrealtime01/init.c
psxclockrealtime01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxclockrealtime01) $(support_includes)
endif

if TEST_psxconcurrency01
psx_tests += psxconcurrency01
psx_screens += psxconcurrency01/psxconcurrency01.scn
psx_docs += psxconcurrency01/psxconcurrency01.doc
psxconcurrency01_SOURCES = psxconcurrency01/init.c include/pmacros.h
psxconcurrency01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxconcurrency01) $(support_includes) \
	-I$(top_srcdir)/include
endif

if TEST_psxcond01
psx_tests += psxcond01
psx_screens += psxcond01/psxcond01.scn
psx_docs += psxcond01/psxcond01.doc
psxcond01_SOURCES = psxcond01/init.c include/pmacros.h
psxcond01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcond01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxcond02
psx_tests += psxcond02
psx_screens += psxcond02/psxcond02.scn
psx_docs += psxcond02/psxcond02.doc
psxcond02_SOURCES = psxcond02/init.c include/pmacros.h
psxcond02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxcond02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxconfig01
psx_tests += psxconfig01
psx_screens += psxconfig01/psxconfig01.scn
psx_docs += psxconfig01/psxconfig01.doc
psxconfig01_SOURCES = psxconfig01/init.c include/pmacros.h
psxconfig01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxconfig01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxdevctl01
psx_tests += psxdevctl01
psx_screens += psxdevctl01/psxdevctl01.scn
psxdevctl01_SOURCES = psxdevctl01/main.c psxdevctl01/test.c \
	include/pmacros.h
psxdevctl01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxdevctl01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psxeintr_join
psx_tests += psxeintr_join
psx_screens += psxeintr_join/psxeintr_join.scn
psx_docs += psxeintr_join/psxeintr_join.doc
psxeintr_join_SOURCES = psxeintr_join/init.c
psxeintr_join_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxeintr_join) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxenosys
psx_tests += psxenosys
psx_screens += psxenosys/psxenosys.scn
psx_docs += psxenosys/psxenosys.doc
psxenosys_SOURCES = psxenosys/init.c psxenosys/system.h \
	include/pmacros.h
psxenosys_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxenosys) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxfatal01
psx_tests += psxfatal01
psx_screens += psxfatal01/psxfatal01.scn
psxfatal01_SOURCES = psxfatal_support/init.c psxfatal_support/system.h \
	psxfatal01/testcase.h
psxfatal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfatal01) \
	$(support_includes) -I$(top_srcdir)/psxfatal01
endif

if TEST_psxfatal02
psx_tests += psxfatal02
psx_screens += psxfatal02/psxfatal02.scn
psxfatal02_SOURCES = psxfatal_support/init.c psxfatal_support/system.h \
	psxfatal02/testcase.h
psxfatal02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfatal02) \
	$(support_includes) -I$(top_srcdir)/psxfatal02
endif

if TEST_psxfchx01
psx_tests += psxfchx01
psx_screens += psxfchx01/psxfchx01.scn
psx_docs += psxfchx01/psxfchx01.doc
psxfchx01_SOURCES = psxfchx01/init.c include/pmacros.h
psxfchx01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfchx01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_FENV
if TEST_psxfenv01
psx_tests += psxfenv01
psx_screens += psxfenv01/psxfenv01.scn
psx_docs += psxfenv01/psxfenv01.doc
psxfenv01_SOURCES = psxfenv01/init.c
psxfenv01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfenv01) \
       $(support_includes)
psxfenv01_LDADD = -lm $(LDADD)
endif
endif

if TEST_psxfile01
psx_tests += psxfile01
psx_screens += psxfile01/psxfile01.scn
psxfile01_SOURCES = psxfile01/main.c psxfile01/test.c \
	psxfile01/test_cat.c psxfile01/test_extend.c psxfile01/test_write.c \
	include/pmacros.h
psxfile01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfile01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxfile02
psx_tests += psxfile02
psx_screens += psxfile02/psxfile02.scn
psx_docs += psxfile02/psxfile02.doc
psxfile02_SOURCES = psxfile02/init.c ../support/src/spin.c
psxfile02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfile02) \
	$(support_includes)
endif

if TEST_psxfilelock01
psx_tests += psxfilelock01
psx_screens += psxfilelock01/psxfilelock01.scn
psx_docs += psxfilelock01/psxfilelock01.doc
psxfilelock01_SOURCES = psxfilelock01/init.c
psxfilelock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxfilelock01) \
	$(support_includes)
endif

if TEST_psxgetattrnp01
psx_tests += psxgetattrnp01
psx_screens += psxgetattrnp01/psxgetattrnp01.scn
psx_docs += psxgetattrnp01/psxgetattrnp01.doc
psxgetattrnp01_SOURCES = psxgetattrnp01/init.c
psxgetattrnp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxgetattrnp01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxgetrusage01
psx_tests += psxgetrusage01
psx_screens += psxgetrusage01/psxgetrusage01.scn
psx_docs += psxgetrusage01/psxgetrusage01.doc
psxgetrusage01_SOURCES = psxgetrusage01/init.c ../support/src/spin.c
psxgetrusage01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxgetrusage01) \
	$(support_includes)
endif

if HAS_CPLUSPLUS
if TEST_psxglobalcon01
psx_tests += psxglobalcon01
psx_screens += psxglobalcon01/psxglobalcon01.scn
psx_docs += psxglobalcon01/psxglobalcon01.doc
psxglobalcon01_SOURCES = psxglobalcon01/init.cc
psxglobalcon01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxglobalcon01) \
	$(support_includes)
endif
endif

if HAS_CPLUSPLUS
if TEST_psxglobalcon02
psx_tests += psxglobalcon02
psx_screens += psxglobalcon02/psxglobalcon02.scn
psx_docs += psxglobalcon02/psxglobalcon02.doc
psxglobalcon02_SOURCES = psxglobalcon02/init.cc
psxglobalcon02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxglobalcon02) \
	$(support_includes)
endif
endif

if TEST_psxid01
psx_tests += psxid01
psx_screens += psxid01/psxid01.scn
psx_docs += psxid01/psxid01.doc
psxid01_SOURCES = psxid01/init.c
psxid01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxid01) \
	$(support_includes)
endif

if TEST_psximfs01
psx_tests += psximfs01
psx_screens += psximfs01/psximfs01.scn
psx_docs += psximfs01/psximfs01.doc
psximfs01_SOURCES = psximfs01/init.c
psximfs01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psximfs01) \
	$(support_includes)
endif

if TEST_psximfs02
psx_tests += psximfs02
psx_screens += psximfs02/psximfs02.scn
psx_docs += psximfs02/psximfs02.doc
psximfs02_SOURCES = psximfs02/init.c
psximfs02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psximfs02) \
	$(support_includes)
endif

if HAS_POSIX
if TEST_psxintrcritical01
psx_tests += psxintrcritical01
psx_screens += psxintrcritical01/psxintrcritical01.scn
psx_docs += psxintrcritical01/psxintrcritical01.doc
psxintrcritical01_SOURCES = psxintrcritical01/init.c \
	../sptests/spintrcritical_support/intrcritical.c
psxintrcritical01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxintrcritical01) $(support_includes) \
	-I$(top_srcdir)/../sptests/spintrcritical_support
endif
endif

if TEST_psxinttypes01
psx_tests += psxinttypes01
psxinttypes01_SOURCES = psxinttypes01/init.c
psxinttypes01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxinttypes01) \
       $(support_includes)
endif

if HAS_POSIX
if TEST_psxitimer
psx_tests += psxitimer
psx_screens += psxitimer/psxitimer.scn
psxitimer_SOURCES = psxitimer/init.c include/pmacros.h
psxitimer_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxitimer) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxkey01
psx_tests += psxkey01
psx_screens += psxkey01/psxkey01.scn
psxkey01_SOURCES = psxkey01/init.c psxkey01/system.h include/pmacros.h \
	../support/src/test_support.c
psxkey01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey02
psx_tests += psxkey02
psx_screens += psxkey02/psxkey02.scn
psx_docs += psxkey02/psxkey02.doc
psxkey02_SOURCES = psxkey02/init.c
psxkey02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey03
psx_tests += psxkey03
psx_screens += psxkey03/psxkey03.scn
psx_docs += psxkey03/psxkey03.doc
psxkey03_SOURCES = psxkey03/init.c
psxkey03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey03) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey04
psx_tests += psxkey04
psx_screens += psxkey04/psxkey04.scn
psx_docs += psxkey04/psxkey04.doc
psxkey04_SOURCES = psxkey04/init.c
psxkey04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey04) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey05
psx_tests += psxkey05
psx_screens += psxkey05/psxkey05.scn
psx_docs += psxkey05/psxkey05.doc
psxkey05_SOURCES = psxkey05/init.c
psxkey05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey05) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey06
psx_tests += psxkey06
psx_screens += psxkey06/psxkey06.scn
psx_docs += psxkey06/psxkey06.doc
psxkey06_SOURCES = psxkey06/init.c
psxkey06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey06) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey07
psx_tests += psxkey07
psx_screens += psxkey07/psxkey07.scn
psx_docs += psxkey07/psxkey07.doc
psxkey07_SOURCES = psxkey07/init.c
psxkey07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey07) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey08
psx_tests += psxkey08
psx_screens += psxkey08/psxkey08.scn
psx_docs += psxkey08/psxkey08.doc
psxkey08_SOURCES = psxkey08/init.c
psxkey08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey08) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey09
psx_tests += psxkey09
psx_screens += psxkey09/psxkey09.scn
psx_docs += psxkey09/psxkey09.doc
psxkey09_SOURCES = psxkey09/init.c
psxkey09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey09) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxkey10
psx_tests += psxkey10
psx_screens += psxkey10/psxkey10.scn
psx_docs += psxkey10/psxkey10.doc
psxkey10_SOURCES = psxkey10/init.c
psxkey10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxkey10) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmmap01
psx_tests += psxmmap01
psx_screens += psxmmap01/psxmmap01.scn
psxmmap01_SOURCES = psxmmap01/init.c psxmmap01/test_helper.c \
	psxmmap01/test_driver.c psxmmap01/system.h
psxmmap01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmmap01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmount
psx_tests += psxmount
psx_screens += psxmount/psxmount.scn
psxmount_SOURCES = psxmount/main.c psxmount/test.c include/pmacros.h
psxmount_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmount) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmsgq01
psx_tests += psxmsgq01
psx_screens += psxmsgq01/psxmsgq01.scn
psxmsgq01_SOURCES = psxmsgq01/init.c psxmsgq01/system.h \
	include/pmacros.h ../support/src/test_support.c
psxmsgq01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmsgq01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmsgq02
psx_tests += psxmsgq02
psx_screens += psxmsgq02/psxmsgq02.scn
psxmsgq02_SOURCES = psxmsgq02/init.c psxmsgq02/system.h \
	include/pmacros.h ../support/src/test_support.c
psxmsgq02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmsgq02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmsgq03
psx_tests += psxmsgq03
psx_screens += psxmsgq03/psxmsgq03.scn
psx_docs += psxmsgq03/psxmsgq03.doc
psxmsgq03_SOURCES = psxmsgq03/init.c psxmsgq03/system.h \
	include/pmacros.h ../support/src/test_support.c
psxmsgq03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmsgq03) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmsgq04
psx_tests += psxmsgq04
psx_screens += psxmsgq04/psxmsgq04.scn
psx_docs += psxmsgq04/psxmsgq04.doc
psxmsgq04_SOURCES = psxmsgq04/init.c include/pmacros.h \
	../support/src/test_support.c
psxmsgq04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmsgq04) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxmutexattr01
psx_tests += psxmutexattr01
psx_screens += psxmutexattr01/psxmutexattr01.scn
psx_docs += psxmutexattr01/psxmutexattr01.doc
psxmutexattr01_SOURCES = psxmutexattr01/init.c
psxmutexattr01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxmutexattr01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_NDBM
if TEST_psxndbm01
psx_tests += psxndbm01
psx_screens += psxndbm01/psxndbm01.scn
psx_docs += psxndbm01/psxndbm01.doc
psxndbm01_SOURCES = psxndbm01/init.c
psxndbm01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxndbm01) \
       $(support_includes)
endif
endif

if TEST_psxobj01
psx_tests += psxobj01
psx_screens += psxobj01/psxobj01.scn
psx_docs += psxobj01/psxobj01.doc
psxobj01_SOURCES = psxobj01/init.c ../support/src/test_support.c
psxobj01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxobj01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxonce01
psx_tests += psxonce01
psx_screens += psxonce01/psxonce01.scn
psxonce01_SOURCES = psxonce01/init.c psxonce01/system.h \
	include/pmacros.h
psxonce01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxonce01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxpasswd01
psx_tests += psxpasswd01
psx_screens += psxpasswd01/psxpasswd01.scn
psx_docs += psxpasswd01/psxpasswd01.doc
psxpasswd01_SOURCES = psxpasswd01/init.c include/pmacros.h
psxpasswd01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxpasswd01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxpasswd02
psx_tests += psxpasswd02
psx_screens += psxpasswd02/psxpasswd02.scn
psx_docs += psxpasswd02/psxpasswd02.doc
psxpasswd02_SOURCES = psxpasswd02/init.c include/pmacros.h
psxpasswd02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxpasswd02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxpipe01
psx_tests += psxpipe01
psx_screens += psxpipe01/psxpipe01.scn
psx_docs += psxpipe01/psxpipe01.doc
psxpipe01_SOURCES = psxpipe01/init.c include/pmacros.h
psxpipe01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxpipe01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxrdwrv
psx_tests += psxrdwrv
psx_screens += psxrdwrv/psxrdwrv.scn
psxrdwrv_SOURCES = psxrdwrv/main.c psxrdwrv/test.c include/pmacros.h
psxrdwrv_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxrdwrv) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxreaddir
psx_tests += psxreaddir
psx_screens += psxreaddir/psxreaddir.scn
psxreaddir_SOURCES = psxreaddir/main.c psxreaddir/test.c \
	include/pmacros.h
psxreaddir_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxreaddir) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxrwlock01
psx_tests += psxrwlock01
psx_screens += psxrwlock01/psxrwlock01.scn
psxrwlock01_SOURCES = psxrwlock01/main.c psxrwlock01/test.c \
	include/pmacros.h
psxrwlock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxrwlock01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxsem01
psx_tests += psxsem01
psx_screens += psxsem01/psxsem01.scn
psxsem01_SOURCES = psxsem01/init.c include/pmacros.h \
	../support/src/test_support.c
psxsem01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsem01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxshm01
psx_tests += psxshm01
psx_screens += psxshm01/psxshm01.scn
psxshm01_SOURCES = psxshm01/init.c psxshm01/system.h
psxshm01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxshm01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxshm02
psx_tests += psxshm02
psx_screens += psxshm02/psxshm02.scn
psxshm02_SOURCES = psxshm02/init.c psxshm02/system.h
psxshm02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxshm02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if HAS_POSIX
if TEST_psxsignal01
psx_tests += psxsignal01
psx_screens += psxsignal01/psxsignal01.scn
psxsignal01_SOURCES = psxsignal01/init.c psxsignal01/task1.c \
	psxsignal01/system.h include/pmacros.h
psxsignal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal01) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxsignal02
psx_tests += psxsignal02
psx_screens += psxsignal02/psxsignal02.scn
psx_docs += psxsignal02/psxsignal02.doc
psxsignal02_SOURCES = psxsignal02/init.c include/pmacros.h
psxsignal02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal02) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxsignal03
psx_tests += psxsignal03
psx_screens += psxsignal03/psxsignal03.scn
psx_docs += psxsignal03/psxsignal03.doc
psxsignal03_SOURCES = psxsignal03/init.c include/pmacros.h
psxsignal03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal03) \
	$(support_includes) -I$(top_srcdir)/include \
	-DUSE_USER_SIGNALS_PROCESS
endif
endif

if HAS_POSIX
if TEST_psxsignal04
psx_tests += psxsignal04
psx_screens += psxsignal04/psxsignal04.scn
psx_docs += psxsignal04/psxsignal04.doc
psxsignal04_SOURCES = psxsignal03/init.c include/pmacros.h
psxsignal04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal04) \
	$(support_includes) -I$(top_srcdir)/include \
	-DUSE_REAL_TIME_SIGNALS_PROCESS
endif
endif

if HAS_POSIX
if TEST_psxsignal05
psx_tests += psxsignal05
psx_screens += psxsignal05/psxsignal05.scn
psx_docs += psxsignal05/psxsignal05.doc
psxsignal05_SOURCES = psxsignal05/init.c include/pmacros.h
psxsignal05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal05) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxsignal06
psx_tests += psxsignal06
psx_screens += psxsignal06/psxsignal06.scn
psx_docs += psxsignal06/psxsignal06.doc
psxsignal06_SOURCES = psxsignal06/init.c
psxsignal06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal06) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxsignal07
psx_tests += psxsignal07
psx_screens += psxsignal07/psxsignal07.scn
psx_docs += psxsignal07/psxsignal07.doc
psxsignal07_SOURCES = psxsignal07/main.c psxsignal07/rtems_config.c
psxsignal07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal07) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxsignal08
psx_tests += psxsignal08
psx_screens += psxsignal08/psxsignal08.scn
psx_docs += psxsignal08/psxsignal08.doc
psxsignal08_SOURCES = psxsignal08/main.c psxsignal08/rtems_config.c
psxsignal08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsignal08) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxspin01
psx_tests += psxspin01
psx_screens += psxspin01/psxspin01.scn
psxspin01_SOURCES = psxspin01/main.c psxspin01/test.c \
	include/pmacros.h
psxspin01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxspin01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxstack01
psx_tests += psxstack01
psx_screens += psxstack01/psxstack01.scn
psx_docs += psxstack01/psxstack01.doc
psxstack01_SOURCES = psxstack01/init.c include/pmacros.h
psxstack01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxstack01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxstack02
psx_tests += psxstack02
psx_screens += psxstack02/psxstack02.scn
psx_docs += psxstack02/psxstack02.doc
psxstack02_SOURCES = psxstack02/init.c
psxstack02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxstack02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxstat
psx_tests += psxstat
psx_screens += psxstat/psxstat.scn
psxstat_SOURCES = psxstat/main.c psxstat/test.c include/pmacros.h
psxstat_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxstat) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxstrsignal01
psx_tests += psxstrsignal01
psx_screens += psxstrsignal01/psxstrsignal01.scn
psx_docs += psxstrsignal01/psxstrsignal01.doc
psxstrsignal01_SOURCES = psxstrsignal01/init.c include/pmacros.h
psxstrsignal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxstrsignal01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxsysconf
psx_tests += psxsysconf
psx_screens += psxsysconf/psxsysconf.scn
psxsysconf_SOURCES = psxsysconf/init.c psxsysconf/system.h \
	include/pmacros.h
psxsysconf_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxsysconf) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_psxthreadname01
psx_tests += psxthreadname01
psx_screens += psxthreadname01/psxthreadname01.scn
psx_docs += psxthreadname01/psxthreadname01.doc
psxthreadname01_SOURCES = psxthreadname01/init.c
psxthreadname01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxthreadname01) $(support_includes)
endif

if TEST_psxtime
psx_tests += psxtime
psx_screens += psxtime/psxtime.scn
psxtime_SOURCES = psxtime/main.c psxtime/test.c
psxtime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtime) \
	$(support_includes)
endif

if HAS_POSIX
if TEST_psxtimer01
psx_tests += psxtimer01
psx_screens += psxtimer01/psxtimer01.scn
psxtimer01_SOURCES = psxtimer01/psxtimer.c psxtimer01/system.h \
	include/pmacros.h
psxtimer01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtimer01) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if HAS_POSIX
if TEST_psxtimer02
psx_tests += psxtimer02
psx_screens += psxtimer02/psxtimer02.scn
psxtimer02_SOURCES = psxtimer02/psxtimer.c psxtimer02/system.h \
	include/pmacros.h
psxtimer02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtimer02) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxtimes01
psx_tests += psxtimes01
psx_screens += psxtimes01/psxtimes01.scn
psx_docs += psxtimes01/psxtimes01.doc
psxtimes01_SOURCES = psxtimes01/init.c ../support/src/spin.c
psxtimes01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtimes01) \
	$(support_includes)
endif

if HAS_POSIX
if TEST_psxualarm
psx_tests += psxualarm
psx_screens += psxualarm/psxualarm.scn
psxualarm_SOURCES = psxualarm/init.c psxualarm/system.h \
	include/pmacros.h
psxualarm_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxualarm) \
	$(support_includes) -I$(top_srcdir)/include
endif
endif

if TEST_psxusleep
psx_tests += psxusleep
psx_screens += psxusleep/psxusleep.scn
psx_docs += psxusleep/psxusleep.doc
psxusleep_SOURCES = psxusleep/init.c include/pmacros.h
psxusleep_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxusleep) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_lib_a
psx_lib += lib.a
lib_a_SOURCES = psxhdrs/devctl/posix_devctl.c \
	psxhdrs/pthread/pthread_attr_destroy.c \
	psxhdrs/pthread/pthread_attr_getdetachstate.c \
	psxhdrs/pthread/pthread_attr_getguardsize.c \
	psxhdrs/pthread/pthread_attr_getinheritsched.c \
	psxhdrs/pthread/pthread_attr_getschedparam.c \
	psxhdrs/pthread/pthread_attr_getschedpolicy.c \
	psxhdrs/pthread/pthread_attr_getscope.c \
	psxhdrs/pthread/pthread_attr_getstackaddr.c \
	psxhdrs/pthread/pthread_attr_getstack.c \
	psxhdrs/pthread/pthread_attr_getstacksize.c \
	psxhdrs/pthread/pthread_attr_init.c \
	psxhdrs/pthread/pthread_attr_setdetachstate.c \
	psxhdrs/pthread/pthread_attr_setguardsize.c \
	psxhdrs/pthread/pthread_attr_setinheritsched.c \
	psxhdrs/pthread/pthread_attr_setschedparam.c \
	psxhdrs/pthread/pthread_attr_setschedpolicy.c \
	psxhdrs/pthread/pthread_attr_setscope.c \
	psxhdrs/pthread/pthread_attr_setstackaddr.c \
	psxhdrs/pthread/pthread_attr_setstack.c \
	psxhdrs/pthread/pthread_attr_setstacksize.c \
	psxhdrs/pthread/pthread_cancel.c psxhdrs/pthread/pthread_cleanup.c \
	psxhdrs/pthread/pthread_condattr_destroy.c \
	psxhdrs/pthread/pthread_condattr_getpshared.c \
	psxhdrs/pthread/pthread_condattr_init.c \
	psxhdrs/pthread/pthread_condattr_setpshared.c \
	psxhdrs/pthread/pthread_cond_broadcast.c \
	psxhdrs/pthread/pthread_cond_destroy.c \
	psxhdrs/pthread/pthread_cond_init.c \
	psxhdrs/pthread/pthread_cond_signal.c \
	psxhdrs/pthread/pthread_cond_timedwait.c \
	psxhdrs/pthread/pthread_cond_wait.c psxhdrs/pthread/pthread_create.c \
	psxhdrs/pthread/pthread_detach.c psxhdrs/pthread/pthread_equal.c \
	psxhdrs/pthread/pthread_exit.c \
	psxhdrs/pthread/pthread_getconcurrency.c \
	psxhdrs/pthread/pthread_getcpuclockid.c \
	psxhdrs/pthread/pthread_getschedparam.c \
	psxhdrs/pthread/pthread_getspecific.c psxhdrs/pthread/pthread_join.c \
	psxhdrs/pthread/pthread_key_create.c \
	psxhdrs/pthread/pthread_key_delete.c \
	psxhdrs/pthread/pthread_mutexattr_destroy.c \
	psxhdrs/pthread/pthread_mutexattr_getprioceiling.c \
	psxhdrs/pthread/pthread_mutexattr_getprotocol.c \
	psxhdrs/pthread/pthread_mutexattr_getpshared.c \
	psxhdrs/pthread/pthread_mutexattr_init.c \
	psxhdrs/pthread/pthread_mutexattr_setprioceiling.c \
	psxhdrs/pthread/pthread_mutexattr_setprotocol.c \
	psxhdrs/pthread/pthread_mutexattr_setpshared.c \
	psxhdrs/pthread/pthread_mutex_destroy.c \
	psxhdrs/pthread/pthread_mutex_getprioceiling.c \
	psxhdrs/pthread/pthread_mutex_init.c \
	psxhdrs/pthread/pthread_mutex_lock.c \
	psxhdrs/pthread/pthread_mutex_setprioceiling.c \
	psxhdrs/pthread/pthread_mutex_timedlock.c \
	psxhdrs/pthread/pthread_mutex_trylock.c \
	psxhdrs/pthread/pthread_mutex_unlock.c psxhdrs/pthread/pthread_once.c \
	psxhdrs/pthread/pthread_self.c \
	psxhdrs/pthread/pthread_setcancelstate.c \
	psxhdrs/pthread/pthread_setcanceltype.c \
	psxhdrs/pthread/pthread_setconcurrency.c \
	psxhdrs/pthread/pthread_setschedparam.c \
	psxhdrs/pthread/pthread_setspecific.c \
	psxhdrs/pthread/pthread_testcancel.c psxhdrs/sched/sched_getparam.c \
	psxhdrs/sched/sched_get_priority_max.c \
	psxhdrs/sched/sched_get_priority_min.c \
	psxhdrs/sched/sched_getscheduler.c \
	psxhdrs/sched/sched_rr_get_interval.c psxhdrs/sched/sched_setparam.c \
	psxhdrs/sched/sched_setscheduler.c psxhdrs/sched/sched_yield.c \
	psxhdrs/signal/pthread_sigmask.c psxhdrs/signal/raise.c \
	psxhdrs/signal/signal.c psxhdrs/signal/sigpending.c \
	psxhdrs/signal/sigprocmask.c psxhdrs/signal/sigsuspend.c \
	psxhdrs/signal/sigtimedwait.c psxhdrs/signal/sigwait.c \
	psxhdrs/signal/sigwaitinfo.c psxhdrs/time/asctime.c \
	psxhdrs/time/asctime_r.c psxhdrs/time/clock.c \
	psxhdrs/time/clock_getcpuclockid.c psxhdrs/time/clock_getres.c \
	psxhdrs/time/clock_gettime.c psxhdrs/time/clock_nanosleep.c \
	psxhdrs/time/clock_settime.c psxhdrs/time/ctime.c \
	psxhdrs/time/ctime_r.c psxhdrs/time/difftime.c psxhdrs/time/gmtime.c \
	psxhdrs/time/gmtime_r.c psxhdrs/time/localtime.c \
	psxhdrs/time/localtime_r.c psxhdrs/time/mktime.c \
	psxhdrs/time/nanosleep.c psxhdrs/time/strftime.c psxhdrs/time/time.c \
	psxhdrs/time/timer_create.c psxhdrs/time/timer_delete.c \
	psxhdrs/time/timer_getoverrun.c psxhdrs/time/timer_gettime.c \
	psxhdrs/time/timer_settime.c psxhdrs/unistd/alarm.c \
	psxhdrs/unistd/fdatasync.c psxhdrs/unistd/fsync.c \
	psxhdrs/unistd/getegid.c psxhdrs/unistd/geteuid.c \
	psxhdrs/unistd/getgid.c psxhdrs/unistd/getgroups.c \
	psxhdrs/unistd/getlogin.c psxhdrs/unistd/getlogin_r.c \
	psxhdrs/unistd/getpgrp.c psxhdrs/unistd/getpid.c \
	psxhdrs/unistd/getppid.c psxhdrs/unistd/getuid.c \
	psxhdrs/unistd/pause.c psxhdrs/unistd/setgid.c \
	psxhdrs/unistd/setgroups.c psxhdrs/unistd/setpgid.c \
	psxhdrs/unistd/setsid.c psxhdrs/unistd/setuid.c \
	psxhdrs/unistd/sleep.c psxhdrs/unistd/sync.c psxhdrs/unistd/ualarm.c \
	psxhdrs/unistd/usleep.c psxhdrs/sys/mman/mlockall.c \
        psxhdrs/utime/utime.c \
        psxhdrs/wordexp/wordexp.c \
        psxhdrs/wordexp/wordfree.c \
	psxhdrs/sys/mman/mlock.c psxhdrs/sys/mman/mmap.c \
	psxhdrs/sys/mman/mprotect.c psxhdrs/sys/mman/msync.c \
	psxhdrs/sys/mman/munlockall.c psxhdrs/sys/mman/munlock.c \
	psxhdrs/sys/mman/munmap.c psxhdrs/sys/mman/posix_madvise.c \
	psxhdrs/sys/mman/shm_open.c psxhdrs/sys/mman/shm_unlink.c \
	psxhdrs/sys/socket/accept.c psxhdrs/sys/socket/bind.c \
	psxhdrs/sys/socket/connect.c psxhdrs/sys/socket/getpeername.c \
	psxhdrs/sys/socket/getsockname.c psxhdrs/sys/socket/getsockopt.c \
	psxhdrs/sys/socket/listen.c psxhdrs/sys/socket/recv.c \
	psxhdrs/sys/socket/recvfrom.c psxhdrs/sys/socket/recvmsg.c \
	psxhdrs/sys/socket/send.c psxhdrs/sys/socket/sendmsg.c \
	psxhdrs/sys/socket/sendto.c psxhdrs/sys/socket/setsockopt.c \
	psxhdrs/sys/socket/shutdown.c psxhdrs/sys/socket/socket.c \
	psxhdrs/sys/socket/socketpair.c \
	psxhdrs/sys/stat/chmod.c \
	psxhdrs/sys/stat/fchmodat.c \
	psxhdrs/sys/stat/fchmod.c \
	psxhdrs/sys/stat/fstatat.c \
	psxhdrs/sys/stat/fstat.c \
	psxhdrs/sys/stat/futimens.c \
	psxhdrs/sys/stat/lstat.c \
	psxhdrs/sys/stat/mkdirat.c \
	psxhdrs/sys/stat/mkdir.c \
	psxhdrs/sys/stat/mkfifoat.c \
	psxhdrs/sys/stat/mkfifo.c \
	psxhdrs/sys/stat/mknodat.c \
	psxhdrs/sys/stat/mknod.c \
	psxhdrs/sys/stat/stat.c \
	psxhdrs/sys/stat/umask.c \
	psxhdrs/sys/stat/utimensat.c \
	psxhdrs/sys/time/select.c \
	psxhdrs/sys/time/utimes.c \
	psxhdrs/sys/time/setitimer.c \
	psxhdrs/sys/time/getitimer.c \
	psxhdrs/sys/time/gettimeofday.c \
	psxhdrs/sys/utsname/uname.c \
	psxhdrs/dirent/alphasort.c \
	psxhdrs/dirent/closedir.c \
	psxhdrs/dirent/opendir.c \
	psxhdrs/dirent/readdir.c \
	psxhdrs/dirent/readdir_r.c \
	psxhdrs/dirent/rewinddir.c \
	psxhdrs/dirent/scandir.c \
	psxhdrs/dirent/seekdir.c \
	psxhdrs/dirent/telldir.c \
	psxhdrs/errno/errno.c \
	psxhdrs/glob/glob.c \
	psxhdrs/glob/globfree.c \
	psxhdrs/libgen/basename.c \
	psxhdrs/libgen/dirname.c \
	psxhdrs/math/acos.c \
	psxhdrs/math/acosf.c \
	psxhdrs/math/acosh.c \
	psxhdrs/math/acoshf.c \
	psxhdrs/math/acoshl.c \
	psxhdrs/math/acosl.c \
	psxhdrs/math/asin.c \
	psxhdrs/math/asinf.c \
	psxhdrs/math/asinh.c \
	psxhdrs/math/asinhf.c \
	psxhdrs/math/asinhl.c \
	psxhdrs/math/asinl.c \
	psxhdrs/math/atan2.c \
	psxhdrs/math/atan2f.c \
	psxhdrs/math/atan2l.c \
	psxhdrs/math/atan.c \
	psxhdrs/math/atanf.c \
	psxhdrs/math/atanh.c \
	psxhdrs/math/atanhf.c \
	psxhdrs/math/atanhl.c \
	psxhdrs/math/atanl.c \
	psxhdrs/math/cbrt.c \
	psxhdrs/math/cbrtf.c \
	psxhdrs/math/cbrtl.c \
	psxhdrs/math/ceil.c \
	psxhdrs/math/ceilf.c \
	psxhdrs/math/ceill.c \
	psxhdrs/math/copysign.c \
	psxhdrs/math/copysignf.c \
	psxhdrs/math/copysignl.c \
	psxhdrs/math/cos.c \
	psxhdrs/math/cosf.c \
	psxhdrs/math/cosh.c \
	psxhdrs/math/coshf.c \
	psxhdrs/math/coshl.c \
	psxhdrs/math/cosl.c \
	psxhdrs/math/erf.c \
	psxhdrs/math/erfc.c \
	psxhdrs/math/erfcf.c \
	psxhdrs/math/erfcl.c \
	psxhdrs/math/erff.c \
	psxhdrs/math/erfl.c \
	psxhdrs/math/exp2.c \
	psxhdrs/math/exp2f.c \
	psxhdrs/math/exp2l.c \
	psxhdrs/math/exp.c \
	psxhdrs/math/expf.c \
	psxhdrs/math/expl.c \
	psxhdrs/math/expm1.c \
	psxhdrs/math/expm1f.c \
	psxhdrs/math/expm1l.c \
	psxhdrs/math/fabs.c \
	psxhdrs/math/fabsf.c \
	psxhdrs/math/fabsl.c \
	psxhdrs/math/fdim.c \
	psxhdrs/math/fdimf.c \
	psxhdrs/math/fdiml.c \
	psxhdrs/math/floor.c \
	psxhdrs/math/floorf.c \
	psxhdrs/math/floorl.c \
	psxhdrs/math/fma.c \
	psxhdrs/math/fmaf.c \
	psxhdrs/math/fmal.c \
	psxhdrs/math/fmax.c \
	psxhdrs/math/fmaxf.c \
	psxhdrs/math/fmaxl.c \
	psxhdrs/math/fmin.c \
	psxhdrs/math/fminf.c \
	psxhdrs/math/fminl.c \
	psxhdrs/math/fmod.c \
	psxhdrs/math/fmodf.c \
	psxhdrs/math/fmodl.c \
	psxhdrs/math/fpclassify.c \
	psxhdrs/math/frexp.c \
	psxhdrs/math/frexpf.c \
	psxhdrs/math/frexpl.c \
	psxhdrs/math/hypot.c \
	psxhdrs/math/hypotf.c \
	psxhdrs/math/hypotl.c \
	psxhdrs/math/ilogb.c \
	psxhdrs/math/ilogbf.c \
	psxhdrs/math/ilogbl.c \
	psxhdrs/math/isfinite.c \
	psxhdrs/math/isgreater.c \
	psxhdrs/math/isgreaterequal.c \
	psxhdrs/math/isinf.c \
	psxhdrs/math/isless.c \
	psxhdrs/math/islessequal.c \
	psxhdrs/math/islessgreater.c \
	psxhdrs/math/isnan.c \
	psxhdrs/math/isnormal.c \
	psxhdrs/math/isunordered.c \
	psxhdrs/math/ldexp.c \
	psxhdrs/math/ldexpf.c \
	psxhdrs/math/ldexpl.c \
	psxhdrs/math/lgamma.c \
	psxhdrs/math/lgammaf.c \
	psxhdrs/math/lgammal.c \
	psxhdrs/math/llrint.c \
	psxhdrs/math/llrintf.c \
	psxhdrs/math/llrintl.c \
	psxhdrs/math/llround.c \
	psxhdrs/math/llroundf.c \
	psxhdrs/math/llroundl.c \
	psxhdrs/math/log10.c \
	psxhdrs/math/log10f.c \
	psxhdrs/math/log10l.c \
	psxhdrs/math/log1p.c \
	psxhdrs/math/log1pf.c \
	psxhdrs/math/log1pl.c \
	psxhdrs/math/log2.c \
	psxhdrs/math/log2f.c \
	psxhdrs/math/log2l.c \
	psxhdrs/math/logb.c \
	psxhdrs/math/logbf.c \
	psxhdrs/math/logbl.c \
	psxhdrs/math/log.c \
	psxhdrs/math/logf.c \
	psxhdrs/math/logl.c \
	psxhdrs/math/lrint.c \
	psxhdrs/math/lrintf.c \
	psxhdrs/math/lrintl.c \
	psxhdrs/math/lround.c \
	psxhdrs/math/lroundf.c \
	psxhdrs/math/lroundl.c \
	psxhdrs/math/modf.c \
	psxhdrs/math/modff.c \
	psxhdrs/math/modfl.c \
	psxhdrs/math/nan.c \
	psxhdrs/math/nanf.c \
	psxhdrs/math/nanl.c \
	psxhdrs/math/nearbyint.c \
	psxhdrs/math/nearbyintf.c \
	psxhdrs/math/nearbyintl.c \
	psxhdrs/math/nextafter.c \
	psxhdrs/math/nextafterf.c \
	psxhdrs/math/nextafterl.c \
	psxhdrs/math/nexttoward.c \
	psxhdrs/math/nexttowardf.c \
	psxhdrs/math/nexttowardl.c \
	psxhdrs/math/pow.c \
	psxhdrs/math/powf.c \
	psxhdrs/math/powl.c \
	psxhdrs/math/remainder.c \
	psxhdrs/math/remainderf.c \
	psxhdrs/math/remainderl.c \
	psxhdrs/math/remquo.c \
	psxhdrs/math/remquof.c \
	psxhdrs/math/remquol.c \
	psxhdrs/math/rint.c \
	psxhdrs/math/rintf.c \
	psxhdrs/math/rintl.c \
	psxhdrs/math/round.c \
	psxhdrs/math/roundf.c \
	psxhdrs/math/roundl.c \
	psxhdrs/math/scalbln.c \
	psxhdrs/math/scalblnf.c \
	psxhdrs/math/scalblnl.c \
	psxhdrs/math/scalbn.c \
	psxhdrs/math/scalbnf.c \
	psxhdrs/math/scalbnl.c \
	psxhdrs/math/signbit.c \
	psxhdrs/math/sin.c \
	psxhdrs/math/sinf.c \
	psxhdrs/math/sinh.c \
	psxhdrs/math/sinhf.c \
	psxhdrs/math/sinhl.c \
	psxhdrs/math/sinl.c \
	psxhdrs/math/sqrt.c \
	psxhdrs/math/sqrtf.c \
	psxhdrs/math/sqrtl.c \
	psxhdrs/math/tan.c \
	psxhdrs/math/tanf.c \
	psxhdrs/math/tanh.c \
	psxhdrs/math/tanhf.c \
	psxhdrs/math/tanhl.c \
	psxhdrs/math/tanl.c \
	psxhdrs/math/tgamma.c \
	psxhdrs/math/tgammaf.c \
	psxhdrs/math/tgammal.c \
	psxhdrs/math/trunc.c \
	psxhdrs/math/truncf.c \
	psxhdrs/math/truncl.c \
	psxhdrs/math/y0.c \
	psxhdrs/math/y0f.c \
	psxhdrs/math/y1.c \
	psxhdrs/math/y1f.c \
	psxhdrs/math/yn.c \
	psxhdrs/math/ynf.c \
	psxhdrs/mqueue/mq_open.c \
	psxhdrs/mqueue/mq_close.c \
	psxhdrs/mqueue/mq_getattr.c \
	psxhdrs/mqueue/mq_setattr.c \
	psxhdrs/mqueue/mq_notify.c \
	psxhdrs/mqueue/mq_receive.c \
	psxhdrs/mqueue/mq_send.c \
	psxhdrs/mqueue/mq_timedreceive.c \
	psxhdrs/mqueue/mq_timedsend.c \
	psxhdrs/mqueue/mq_unlink.c \
	psxhdrs/semaphore/sem_init.c \
	psxhdrs/semaphore/sem_destroy.c \
	psxhdrs/semaphore/sem_open.c \
	psxhdrs/semaphore/sem_close.c \
	psxhdrs/semaphore/sem_getvalue.c \
	psxhdrs/semaphore/sem_unlink.c \
	psxhdrs/semaphore/sem_post.c \
	psxhdrs/semaphore/sem_wait.c \
	psxhdrs/semaphore/sem_trywait.c \
	psxhdrs/semaphore/sem_timedwait.c \
	psxhdrs/stddef/offsetof.c \
	psxhdrs/stdio/clearerr.c \
	psxhdrs/stdio/ctermid.c \
	psxhdrs/stdio/dprintf.c \
	psxhdrs/stdio/fclose.c \
	psxhdrs/stdio/fdopen.c \
	psxhdrs/stdio/feof.c \
	psxhdrs/stdio/ferror.c \
	psxhdrs/stdio/fflush.c \
	psxhdrs/stdio/fgetc.c \
	psxhdrs/stdio/fgetpos.c \
	psxhdrs/stdio/fgets.c \
	psxhdrs/stdio/fileno.c \
	psxhdrs/stdio/flockfile.c \
	psxhdrs/stdio/fmemopen.c \
	psxhdrs/stdio/fopen.c \
	psxhdrs/stdio/fprintf.c \
	psxhdrs/stdio/fputc.c \
	psxhdrs/stdio/fputs.c \
	psxhdrs/stdio/fread.c \
	psxhdrs/stdio/freopen.c \
	psxhdrs/stdio/fscanf.c \
	psxhdrs/stdio/fseek.c \
	psxhdrs/stdio/fseeko.c \
	psxhdrs/stdio/fsetpos.c \
	psxhdrs/stdio/ftell.c \
	psxhdrs/stdio/ftello.c \
	psxhdrs/stdio/ftrylockfile.c \
	psxhdrs/stdio/funlockfile.c \
	psxhdrs/stdio/fwrite.c \
	psxhdrs/stdio/getc.c \
	psxhdrs/stdio/getc_unlocked.c \
	psxhdrs/stdio/getchar.c \
	psxhdrs/stdio/getchar_unlocked.c \
	psxhdrs/stdio/gets.c \
	psxhdrs/stdio/open_memstream.c \
	psxhdrs/stdio/perror.c \
	psxhdrs/stdio/printf.c \
	psxhdrs/stdio/putc.c \
	psxhdrs/stdio/putc_unlocked.c \
	psxhdrs/stdio/putchar.c \
	psxhdrs/stdio/putchar_unlocked.c \
	psxhdrs/stdio/puts.c \
	psxhdrs/stdio/remove.c \
	psxhdrs/stdio/rename.c \
	psxhdrs/stdio/rewind.c \
	psxhdrs/stdio/scanf.c \
	psxhdrs/stdio/setbuf.c \
	psxhdrs/stdio/setvbuf.c \
	psxhdrs/stdio/snprintf.c \
	psxhdrs/stdio/sprintf.c \
	psxhdrs/stdio/sscanf.c \
	psxhdrs/stdio/stderr.c \
	psxhdrs/stdio/stdin.c \
	psxhdrs/stdio/stdout.c \
	psxhdrs/stdio/tempnam.c \
	psxhdrs/stdio/tmpfile.c \
	psxhdrs/stdio/tmpnam.c \
	psxhdrs/stdio/ungetc.c \
	psxhdrs/stdio/vdprintf.c \
	psxhdrs/stdio/vfprintf.c \
	psxhdrs/stdio/vfscanf.c \
	psxhdrs/stdio/vprintf.c \
	psxhdrs/stdio/vscanf.c \
	psxhdrs/stdio/vsnprintf.c \
	psxhdrs/stdio/vsprintf.c \
	psxhdrs/stdio/vsscanf.c \
	psxhdrs/stdio/pclose.c \
	psxhdrs/stdio/popen.c \
	psxhdrs/string/memccpy.c \
	psxhdrs/string/memchr.c \
	psxhdrs/string/memcmp.c \
	psxhdrs/string/memcpy.c \
	psxhdrs/string/memmove.c \
	psxhdrs/string/memset.c \
	psxhdrs/string/stpcpy.c \
	psxhdrs/string/stpncpy.c \
	psxhdrs/string/strcat.c \
	psxhdrs/string/strchr.c \
	psxhdrs/string/strcmp.c \
	psxhdrs/string/strcoll.c \
	psxhdrs/string/strcoll_l.c \
	psxhdrs/string/strcpy.c \
	psxhdrs/string/strcspn.c \
	psxhdrs/string/strdup.c \
	psxhdrs/string/strerror.c \
	psxhdrs/string/strerror_l.c \
	psxhdrs/string/strerror_r.c \
	psxhdrs/string/strlen.c \
	psxhdrs/string/strncat.c \
	psxhdrs/string/strncmp.c \
	psxhdrs/string/strncpy.c \
	psxhdrs/string/strndup.c \
	psxhdrs/string/strnlen.c \
	psxhdrs/string/strpbrk.c \
	psxhdrs/string/strrchr.c \
	psxhdrs/string/strsignal.c \
	psxhdrs/string/strspn.c \
	psxhdrs/string/strstr.c \
	psxhdrs/string/strtok.c \
	psxhdrs/string/strtok_r.c \
	psxhdrs/string/strxfrm.c \
	psxhdrs/string/strxfrm_l.c \
	psxhdrs/strings/bcmp.c \
	psxhdrs/strings/bcopy.c \
	psxhdrs/strings/ffs.c \
	psxhdrs/strings/index.c \
	psxhdrs/strings/rindex.c \
	psxhdrs/strings/strcasecmp.c \
	psxhdrs/strings/strcasecmp_l.c \
	psxhdrs/strings/strncasecmp.c \
	psxhdrs/strings/strncasecmp_l.c \
	psxhdrs/dlfcn/dlopen.c \
	psxhdrs/dlfcn/dlclose.c \
	psxhdrs/dlfcn/dlerror.c \
	psxhdrs/dlfcn/dlsym.c \
	psxhdrs/fcntl/creat.c \
	psxhdrs/fcntl/fcntl.c \
	psxhdrs/fcntl/open.c \
	psxhdrs/fcntl/openat.c \
	psxhdrs/locale/newlocale.c \
	psxhdrs/locale/freelocale.c \
	psxhdrs/locale/uselocale.c \
	psxhdrs/locale/setlocale.c \
	psxhdrs/locale/duplocale.c \
	psxhdrs/locale/localeconv.c \
	psxhdrs/regex/regcomp.c \
	psxhdrs/regex/regerror.c \
	psxhdrs/regex/regexec.c \
	psxhdrs/regex/regfree.c \
	psxhdrs/fnmatch/fnmatch.c \
	psxhdrs/iconv/iconv.c \
	psxhdrs/iconv/iconv_close.c \
	psxhdrs/iconv/iconv_open.c \
	psxhdrs/regex/regfree.c \
	psxhdrs/langinfo/nl_langinfo.c \
	psxhdrs/langinfo/nl_langinfo_l.c \
	psxhdrs/assert/assert.c \
	psxhdrs/complex/cabs.c \
	psxhdrs/complex/cabsf.c \
	psxhdrs/complex/cabsl.c \
	psxhdrs/complex/cacos.c \
	psxhdrs/complex/cacosf.c \
	psxhdrs/complex/cacosh.c \
	psxhdrs/complex/cacoshf.c \
	psxhdrs/complex/cacoshl.c \
	psxhdrs/complex/cacosl.c \
	psxhdrs/complex/carg.c \
	psxhdrs/complex/cargf.c \
	psxhdrs/complex/cargl.c \
	psxhdrs/complex/casin.c \
	psxhdrs/complex/casinf.c \
	psxhdrs/complex/casinh.c \
	psxhdrs/complex/casinhf.c \
	psxhdrs/complex/casinhl.c \
	psxhdrs/complex/casinl.c \
	psxhdrs/complex/catan.c \
	psxhdrs/complex/catanf.c \
	psxhdrs/complex/catanh.c \
	psxhdrs/complex/catanhf.c \
	psxhdrs/complex/catanhl.c \
	psxhdrs/complex/catanl.c \
	psxhdrs/complex/ccos.c \
	psxhdrs/complex/ccosf.c \
	psxhdrs/complex/ccosh.c \
	psxhdrs/complex/ccoshf.c \
	psxhdrs/complex/ccoshl.c \
	psxhdrs/complex/ccosl.c \
	psxhdrs/complex/cexp.c \
	psxhdrs/complex/cexpf.c \
	psxhdrs/complex/cexpl.c \
	psxhdrs/complex/cimag.c \
	psxhdrs/complex/cimagf.c \
	psxhdrs/complex/cimagl.c \
	psxhdrs/complex/clog.c \
	psxhdrs/complex/clogf.c \
	psxhdrs/complex/clogl.c \
	psxhdrs/complex/conj.c \
	psxhdrs/complex/conjf.c \
	psxhdrs/complex/conjl.c \
	psxhdrs/complex/cpow.c \
	psxhdrs/complex/cpowf.c \
	psxhdrs/complex/cpowl.c \
	psxhdrs/complex/cproj.c \
	psxhdrs/complex/cprojf.c \
	psxhdrs/complex/cprojl.c \
	psxhdrs/complex/creal.c \
	psxhdrs/complex/crealf.c \
	psxhdrs/complex/creall.c \
	psxhdrs/complex/csin.c \
	psxhdrs/complex/csinf.c \
	psxhdrs/complex/csinh.c \
	psxhdrs/complex/csinhf.c \
	psxhdrs/complex/csinhl.c \
	psxhdrs/complex/csinl.c \
	psxhdrs/complex/csqrt.c \
	psxhdrs/complex/csqrtf.c \
	psxhdrs/complex/csqrtl.c \
	psxhdrs/complex/ctan.c \
	psxhdrs/complex/ctanf.c \
	psxhdrs/complex/ctanh.c \
	psxhdrs/complex/ctanhf.c \
	psxhdrs/complex/ctanhl.c \
	psxhdrs/complex/ctanl.c \
	psxhdrs/complex/ctanl.c \
	psxhdrs/ctype/isalnum.c \
	psxhdrs/ctype/isalnum_l.c \
	psxhdrs/ctype/isalpha.c \
	psxhdrs/ctype/isalpha_l.c \
	psxhdrs/ctype/isascii.c \
	psxhdrs/ctype/isblank.c \
	psxhdrs/ctype/isblank_l.c \
	psxhdrs/ctype/iscntrl.c \
	psxhdrs/ctype/iscntrl_l.c \
	psxhdrs/ctype/isdigit.c \
	psxhdrs/ctype/isdigit_l.c \
	psxhdrs/ctype/isgraph.c \
	psxhdrs/ctype/isgraph_l.c \
	psxhdrs/ctype/islower.c \
	psxhdrs/ctype/islower_l.c \
	psxhdrs/ctype/isprint.c \
	psxhdrs/ctype/isprint_l.c \
	psxhdrs/ctype/ispunct.c \
	psxhdrs/ctype/ispunct_l.c \
	psxhdrs/ctype/isspace.c \
	psxhdrs/ctype/isspace_l.c \
	psxhdrs/ctype/isupper.c \
	psxhdrs/ctype/isupper_l.c \
	psxhdrs/ctype/isxdigit.c \
	psxhdrs/ctype/isxdigit_l.c \
	psxhdrs/ctype/toascii.c \
	psxhdrs/ctype/tolower.c \
	psxhdrs/ctype/tolower_l.c \
	psxhdrs/ctype/toupper.c \
	psxhdrs/ctype/toupper_l.c \
	psxhdrs/wctype/iswalnum.c \
	psxhdrs/wctype/iswalnum_l.c \
	psxhdrs/wctype/iswalpha.c \
	psxhdrs/wctype/iswalpha_l.c \
	psxhdrs/wctype/iswblank.c \
	psxhdrs/wctype/iswblank_l.c \
	psxhdrs/wctype/iswcntrl.c \
	psxhdrs/wctype/iswcntrl_l.c \
	psxhdrs/wctype/iswctype.c \
	psxhdrs/wctype/iswctype_l.c \
	psxhdrs/wctype/iswdigit.c \
	psxhdrs/wctype/iswdigit_l.c \
	psxhdrs/wctype/iswgraph.c \
	psxhdrs/wctype/iswgraph_l.c \
	psxhdrs/wctype/iswlower.c \
	psxhdrs/wctype/iswlower_l.c \
	psxhdrs/wctype/iswprint.c \
	psxhdrs/wctype/iswprint_l.c \
	psxhdrs/wctype/iswpunct.c \
	psxhdrs/wctype/iswpunct_l.c \
	psxhdrs/wctype/iswspace.c \
	psxhdrs/wctype/iswspace_l.c \
	psxhdrs/wctype/iswupper.c \
	psxhdrs/wctype/iswupper_l.c \
	psxhdrs/wctype/iswxdigit.c \
	psxhdrs/wctype/iswxdigit_l.c \
	psxhdrs/wctype/towctrans.c \
	psxhdrs/wctype/towctrans_l.c \
	psxhdrs/wctype/towlower.c \
	psxhdrs/wctype/towlower_l.c \
	psxhdrs/wctype/wctrans.c \
	psxhdrs/wctype/wctrans_l.c \
	psxhdrs/wctype/wctype.c \
	psxhdrs/wctype/wctype_l.c \
	psxhdrs/inttypes/imaxabs.c \
	psxhdrs/inttypes/imaxdiv.c \
	psxhdrs/inttypes/strtoimax.c \
	psxhdrs/inttypes/strtoumax.c \
	psxhdrs/inttypes/wcstoimax.c \
	psxhdrs/inttypes/wcstoumax.c \
	psxhdrs/netdb/endhostent.c \
	psxhdrs/netdb/endnetent.c \
	psxhdrs/netdb/endprotoent.c \
	psxhdrs/netdb/endservent.c \
	psxhdrs/netdb/sethostent.c \
	psxhdrs/netdb/setnetent.c \
	psxhdrs/netdb/setservent.c \
	psxhdrs/netdb/gethostent.c \
	psxhdrs/netdb/getprotoent.c \
	psxhdrs/netdb/getnetent.c \
	psxhdrs/netdb/getservent.c \
	psxhdrs/netdb/freeaddrinfo.c \
	psxhdrs/netdb/gai_strerror.c \
	psxhdrs/netdb/getaddrinfo.c \
	psxhdrs/netdb/gethostbyaddr.c \
	psxhdrs/netdb/gethostbyname.c \
	psxhdrs/netdb/getnameinfo.c \
	psxhdrs/netdb/getnetbyname.c \
	psxhdrs/netdb/getprotobyname.c \
	psxhdrs/netdb/getprotobynumber.c \
	psxhdrs/netdb/getservbyname.c \
	psxhdrs/netdb/getservbyport.c \
	psxhdrs/netdb/h_errno.c \
	psxhdrs/net/if/if_freenameindex.c \
	psxhdrs/net/if/if_indextoname.c \
	psxhdrs/net/if/if_nameindex.c \
	psxhdrs/net/if/if_nametoindex.c \
	psxhdrs/arpa/inet/inet_addr.c \
	psxhdrs/arpa/inet/inet_ntop.c \
	psxhdrs/arpa/inet/inet_ntoa.c \
	psxhdrs/arpa/inet/inet_pton.c \
	psxhdrs/pwd/endpwent.c \
	psxhdrs/pwd/getpwent.c \
	psxhdrs/pwd/getpwnam.c \
	psxhdrs/pwd/getpwnam_r.c \
	psxhdrs/pwd/setpwent.c \
	psxhdrs/pwd/getpwuid.c \
	psxhdrs/pwd/getpwuid_r.c \
	psxhdrs/syslog/closelog.c \
	psxhdrs/syslog/openlog.c \
	psxhdrs/syslog/setlogmask.c \
	psxhdrs/syslog/syslog.c \
	psxhdrs/c11/threads/call_once.c \
	psxhdrs/c11/threads/cnd_broadcast.c \
	psxhdrs/c11/threads/cnd_destroy.c \
	psxhdrs/c11/threads/cnd_init.c \
	psxhdrs/c11/threads/cnd_signal.c \
	psxhdrs/c11/threads/cnd_timedwait.c \
	psxhdrs/c11/threads/cnd_wait.c \
	psxhdrs/c11/threads/mtx_init.c \
	psxhdrs/c11/threads/mtx_destroy.c \
	psxhdrs/c11/threads/mtx_lock.c \
	psxhdrs/c11/threads/mtx_timedlock.c \
	psxhdrs/c11/threads/mtx_trylock.c \
	psxhdrs/c11/threads/mtx_unlock.c \
	psxhdrs/c11/threads/thrd_create.c \
	psxhdrs/c11/threads/thrd_current.c \
	psxhdrs/c11/threads/thrd_detach.c \
	psxhdrs/c11/threads/thrd_equal.c \
	psxhdrs/c11/threads/thrd_exit.c \
	psxhdrs/c11/threads/thrd_join.c \
	psxhdrs/c11/threads/thrd_sleep.c \
	psxhdrs/c11/threads/thrd_yield.c \
	psxhdrs/c11/threads/tss_create.c \
	psxhdrs/c11/threads/tss_delete.c \
	psxhdrs/c11/threads/tss_get.c \
	psxhdrs/c11/threads/tss_set.c \
	psxhdrs/wchar/btowc.c \
	psxhdrs/wchar/fgetwc.c \
	psxhdrs/wchar/fgetws.c \
	psxhdrs/wchar/fputwc.c \
	psxhdrs/wchar/fputws.c \
	psxhdrs/wchar/fwide.c \
	psxhdrs/wchar/fwprintf.c \
	psxhdrs/wchar/fwscanf.c \
	psxhdrs/wchar/getwc.c \
	psxhdrs/wchar/getwchar.c \
	psxhdrs/wchar/iswalnum.c \
	psxhdrs/wchar/iswalpha.c \
	psxhdrs/wchar/iswcntrl.c \
	psxhdrs/wchar/iswctype.c \
	psxhdrs/wchar/iswdigit.c \
	psxhdrs/wchar/iswgraph.c \
	psxhdrs/wchar/iswlower.c \
	psxhdrs/wchar/iswprint.c \
	psxhdrs/wchar/iswpunct.c \
	psxhdrs/wchar/iswspace.c \
	psxhdrs/wchar/iswupper.c \
	psxhdrs/wchar/iswxdigit.c \
	psxhdrs/wchar/mbrlen.c \
	psxhdrs/wchar/mbrtowc.c \
	psxhdrs/wchar/mbsinit.c \
	psxhdrs/wchar/mbsnrtowcs.c \
	psxhdrs/wchar/mbsrtowcs.c \
	psxhdrs/wchar/open_wmemstream.c \
	psxhdrs/wchar/putwc.c \
	psxhdrs/wchar/putwchar.c \
	psxhdrs/wchar/swprintf.c \
	psxhdrs/wchar/swscanf.c \
	psxhdrs/wchar/towlower.c \
	psxhdrs/wchar/towupper.c \
	psxhdrs/wchar/ungetwc.c \
	psxhdrs/wchar/vfwprintf.c \
	psxhdrs/wchar/vfwscanf.c \
	psxhdrs/wchar/vswprintf.c \
	psxhdrs/wchar/vswscanf.c \
	psxhdrs/wchar/vwprintf.c \
	psxhdrs/wchar/vwscanf.c \
	psxhdrs/wchar/wcpcpy.c \
	psxhdrs/wchar/wcpncpy.c \
	psxhdrs/wchar/wcrtomb.c \
	psxhdrs/wchar/wcscasecmp.c \
	psxhdrs/wchar/wcscasecmp_l.c \
	psxhdrs/wchar/wcscat.c \
	psxhdrs/wchar/wcschr.c \
	psxhdrs/wchar/wcscmp.c \
	psxhdrs/wchar/wcscoll.c \
	psxhdrs/wchar/wcscoll_l.c \
	psxhdrs/wchar/wcscpy.c \
	psxhdrs/wchar/wcscspn.c \
	psxhdrs/wchar/wcsdup.c \
	psxhdrs/wchar/wcsnlen.c \
	psxhdrs/wchar/wcsnrtombs.c \
	psxhdrs/wchar/wcspbrk.c \
	psxhdrs/wchar/wcsrchr.c \
	psxhdrs/wchar/wcsrtombs.c \
	psxhdrs/wchar/wcsspn.c \
	psxhdrs/wchar/wcsstr.c \
	psxhdrs/wchar/wcstod.c \
	psxhdrs/wchar/wcstof.c \
	psxhdrs/wchar/wcstok.c \
	psxhdrs/wchar/wcstol.c \
	psxhdrs/wchar/wcstold.c \
	psxhdrs/wchar/wcstoll.c \
	psxhdrs/wchar/wcstoul.c \
	psxhdrs/wchar/wcstoull.c \
	psxhdrs/wchar/wcswidth.c \
	psxhdrs/wchar/wcsxfrm.c \
	psxhdrs/wchar/wcsxfrm_l.c \
	psxhdrs/wchar/wctob.c \
	psxhdrs/wchar/wctype.c \
	psxhdrs/wchar/wcwidth.c \
	psxhdrs/wchar/wmemchr.c \
	psxhdrs/wchar/wmemcmp.c \
	psxhdrs/wchar/wmemcpy.c \
	psxhdrs/wchar/wmemmove.c \
	psxhdrs/wchar/wmemset.c \
	psxhdrs/wchar/wprintf.c \
	psxhdrs/wchar/wscanf.c \
	psxhdrs/stdlib/_Exit.c \
	psxhdrs/stdlib/a64l.c \
	psxhdrs/stdlib/abort.c \
	psxhdrs/stdlib/abs.c \
	psxhdrs/stdlib/atexit.c \
	psxhdrs/stdlib/atof.c \
	psxhdrs/stdlib/atoi.c \
	psxhdrs/stdlib/atol.c \
	psxhdrs/stdlib/atoll.c \
	psxhdrs/stdlib/bsearch.c \
	psxhdrs/stdlib/calloc.c \
	psxhdrs/stdlib/div.c \
	psxhdrs/stdlib/drand48.c \
	psxhdrs/stdlib/erand48.c \
	psxhdrs/stdlib/exit.c \
	psxhdrs/stdlib/free.c \
	psxhdrs/stdlib/getenv.c \
	psxhdrs/stdlib/initstate.c \
	psxhdrs/stdlib/jrand48.c \
	psxhdrs/stdlib/l64a.c \
	psxhdrs/stdlib/labs.c \
	psxhdrs/stdlib/lcong48.c \
	psxhdrs/stdlib/ldiv.c \
	psxhdrs/stdlib/llabs.c \
	psxhdrs/stdlib/lldiv.c \
	psxhdrs/stdlib/lrand48.c \
	psxhdrs/stdlib/malloc.c \
	psxhdrs/stdlib/mblen.c \
	psxhdrs/stdlib/mbstowcs.c \
	psxhdrs/stdlib/mbtowc.c \
	psxhdrs/stdlib/mkdtemp.c \
	psxhdrs/stdlib/mkstemp.c \
	psxhdrs/stdlib/mrand48.c \
	psxhdrs/stdlib/nrand48.c \
	psxhdrs/stdlib/posix_memalign.c \
	psxhdrs/stdlib/putenv.c \
	psxhdrs/stdlib/qsort.c \
	psxhdrs/stdlib/rand.c \
	psxhdrs/stdlib/rand_r.c \
	psxhdrs/stdlib/random.c \
	psxhdrs/stdlib/realloc.c \
	psxhdrs/stdlib/realpath.c \
	psxhdrs/stdlib/seed48.c \
	psxhdrs/stdlib/setenv.c \
	psxhdrs/stdlib/setkey.c \
	psxhdrs/stdlib/setstate.c \
	psxhdrs/stdlib/srand.c \
	psxhdrs/stdlib/srand48.c \
	psxhdrs/stdlib/srandom.c \
	psxhdrs/stdlib/strtod.c \
	psxhdrs/stdlib/strtof.c \
	psxhdrs/stdlib/strtol.c \
	psxhdrs/stdlib/strtold.c \
	psxhdrs/stdlib/strtoll.c \
	psxhdrs/stdlib/strtoul.c \
	psxhdrs/stdlib/strtoull.c \
	psxhdrs/stdlib/system.c \
	psxhdrs/stdlib/unsetenv.c \
	psxhdrs/stdlib/wcstombs.c \
	psxhdrs/stdlib/wctomb.c \
	psxhdrs/sys/times/times.c \
	psxhdrs/sys/resource/getrusage.c \
	psxhdrs/setjmp/longjmp.c \
	psxhdrs/setjmp/setjmp.c \
	psxhdrs/setjmp/siglongjmp.c \
	psxhdrs/setjmp/sigsetjmp.c \
	psxhdrs/sys/select/FD_CLR.c \
	psxhdrs/sys/select/FD_ISSET.c \
	psxhdrs/sys/select/FD_SET.c \
	psxhdrs/sys/select/FD_ZERO.c \
	psxhdrs/sys/select/pselect.c \
	psxhdrs/sys/select/select.c \
        psxhdrs/termios/cfgetispeed.c \
	psxhdrs/termios/cfgetospeed.c \
	psxhdrs/termios/cfsetispeed.c \
	psxhdrs/termios/cfsetospeed.c \
	psxhdrs/termios/tcdrain.c \
	psxhdrs/termios/tcflow.c \
	psxhdrs/termios/tcflush.c \
	psxhdrs/termios/tcgetattr.c \
	psxhdrs/termios/tcgetsid.c \
	psxhdrs/termios/tcsendbreak.c \
	psxhdrs/termios/tcsetattr.c

## Not supported by RTEMS, but POSIX API Compliance tests exist.
## lib_a_SOURCES += psxhdrs/ulimit/ulimit.c
## lib_a_SOURCES += psxhdrs/monetary/strfmon.c
## lib_a_SOURCES += psxhdrs/monetary/strfmon_l.c

## Specific issues that tickets are tracking
## lib_a_SOURCES += psxhdrs/dirent/dirfd.c               See ticket #3371
## lib_a_SOURCES += psxhdrs/dirent/fdopendir.c           See ticket #3371
## lib_a_SOURCES += psxhdrs/fcntl/posix_fadvise.c        See ticket #3656
## lib_a_SOURCES += psxhdrs/fcntl/posix_fallocate.c      See ticket #3656
## lib_a_SOURCES += psxhdrs/fenv/feclearexcept.c         See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fegetenv.c              See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fegetexceptflag.c       See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fegetround.c            See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/feholdexcept.c          See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/feraiseexcept.c         See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fesetenv.c              See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fesetexceptflag.c       See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fesetround.c            See ticket #2971
## lib_a_SOURCES += psxhdrs/fenv/fetestexcept.c          See ticket #2971
## lib_a_SOURCES += psxhdrs/fmtmsg/fmtmsg.c              See ticket #3639
## lib_a_SOURCES += psxhdrs/ftw/ftw.c                    See ticket #2970
## lib_a_SOURCES += psxhdrs/math/y0l.c                   See ticket #3638
## lib_a_SOURCES += psxhdrs/math/y1l.c                   See ticket #3638
## lib_a_SOURCES += psxhdrs/math/ynl.c                   See ticket #3638
## lib_a_SOURCES += psxhdrs/poll/poll.c                  See ticket #3657
## lib_a_SOURCES += psxhdrs/ftw/nftw.c                   See ticket #2970
## lib_a_SOURCES += psxhdrs/stdio/getdelim.c	         See ticket #3633
## lib_a_SOURCES += psxhdrs/stdio/getline.c              See ticket #3633
## lib_a_SOURCES += psxhdrs/ucontext/getcontext.c        See ticket #3640
## lib_a_SOURCES += psxhdrs/ucontext/makecontext.c       See ticket #3640
## lib_a_SOURCES += psxhdrs/ucontext/setcontext.c        See ticket #3640
## lib_a_SOURCES += psxhdrs/ucontext/swapcontext.c       See ticket #3640
## lib_a_SOURCES += psxhdrs/c11/uchar/c16rtomb.c         See ticket #3643
## lib_a_SOURCES += psxhdrs/c11/uchar/c32rtomb.c         See ticket #3643
## lib_a_SOURCES += psxhdrs/c11/uchar/c32rtomb.c         See ticket #3643
## lib_a_SOURCES += psxhdrs/c11/uchar/mbrtoc32.c         See ticket #3643
## lib_a_SOURCES += psxhdrs/stdlib/getsubopt.c 	         See ticket #3644
## lib_a_SOURCES += psxhdrs/stdlib/grantpt.c 	         See ticket #3645
## lib_a_SOURCES += psxhdrs/stdlib/posix_openpt.c        See ticket #3646
## lib_a_SOURCES += psxhdrs/stdlib/posix_openpt.c        See ticket #3646
## lib_a_SOURCES += psxhdrs/stdlib/ptsname.c             See ticket #3647
## lib_a_SOURCES += psxhdrs/stdlib/unlockpt.c            See ticket #3648
## lib_a_SOURCES += psxhdrs/sys/ftok.c                   See ticket #3650
## lib_a_SOURCES += psxhdrs/sys/msg/msgctl.c             See ticket #3658
## lib_a_SOURCES += psxhdrs/sys/msg/msgget.c             See ticket #3658
## lib_a_SOURCES += psxhdrs/sys/msg/msgrcv.c             See ticket #3658
## lib_a_SOURCES += psxhdrs/sys/msg/msgsnd.c             See ticket #3658
## lib_a_SOURCES += psxhdrs/sys/resource/getpriority.c   See ticket #3652
## lib_a_SOURCES += psxhdrs/sys/resource/getrlimit.c     See ticket #3653
## lib_a_SOURCES += psxhdrs/sys/resource/setpriority.c   See ticket #3654
## lib_a_SOURCES += psxhdrs/sys/resource/setrlimit.c     See ticket #3655
## lib_a_SOURCES += psxhdrs/sys/shm/shmat.c              See ticket #3662
## lib_a_SOURCES += psxhdrs/sys/shm/shmctl.c             See ticket #3662
## lib_a_SOURCES += psxhdrs/sys/shm/shmdt.c              See ticket #3662
## lib_a_SOURCES += psxhdrs/sys/shm/shmget.c             See ticket #3662
## lib_a_SOURCES += psxhdrs/sys/statvfs/statvfs.c        See ticket #3660
## lib_a_SOURCES += psxhdrs/sys/statvfs/fstatvfs.c       See ticket #3660
## lib_a_SOURCES += psxhdrs/utmpx/setutxent.c            See ticket #3661
## lib_a_SOURCES += psxhdrs/utmpx/endutxent.c            See ticket #3661
## lib_a_SOURCES += psxhdrs/utmpx/getutxent.c            See ticket #3661
## lib_a_SOURCES += psxhdrs/utmpx/getutxid.c             See ticket #3661
## lib_a_SOURCES += psxhdrs/utmpx/getutxline.c           See ticket #3661
## lib_a_SOURCES += psxhdrs/utmpx/pututxline.c           See ticket #3661
endif

noinst_PROGRAMS = $(psx_tests)
noinst_LIBRARIES = $(psx_lib)
