#
# SMP Teststuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

smp_tests =
smp_screens =
smp_docs =

support_includes = -I$(top_srcdir)/../support/include

if HAS_SMP
if TEST_smp01
smp_tests += smp01
smp_screens += smp01/smp01.scn
smp_docs += smp01/smp01.doc
smp01_SOURCES = smp01/init.c smp01/tasks.c smp01/system.h \
	../support/src/locked_print.c
smp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp01) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp02
smp_tests += smp02
smp_screens += smp02/smp02.scn
smp_docs += smp02/smp02.doc
smp02_SOURCES = smp02/init.c smp02/tasks.c smp02/system.h \
	../support/src/locked_print.c
smp02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp02) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp03
smp_tests += smp03
smp_screens += smp03/smp03.scn
smp_docs += smp03/smp03.doc
smp03_SOURCES = smp03/init.c smp03/tasks.c smp03/system.h \
	../support/src/locked_print.c
smp03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp03) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp05
smp_tests += smp05
smp_screens += smp05/smp05.scn
smp_docs += smp05/smp05.doc
smp05_SOURCES = smp05/init.c ../support/src/locked_print.c
smp05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp05) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp07
smp_tests += smp07
smp_screens += smp07/smp07.scn
smp_docs += smp07/smp07.doc
smp07_SOURCES = smp07/init.c ../support/src/locked_print.c
smp07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp07) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp08
smp_tests += smp08
smp_screens += smp08/smp08.scn
smp_docs += smp08/smp08.doc
smp08_SOURCES = smp08/init.c smp08/tasks.c smp08/system.h \
	../support/src/locked_print.c
smp08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp08) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smp09
smp_tests += smp09
smp_screens += smp09/smp09.scn
smp_docs += smp09/smp09.doc
smp09_SOURCES = smp09/init.c ../support/src/locked_print.c
smp09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smp09) \
	$(support_includes) -DSMPTEST
endif
endif

if HAS_SMP
if TEST_smpaffinity01
smp_tests += smpaffinity01
smp_screens += smpaffinity01/smpaffinity01.scn
smp_docs += smpaffinity01/smpaffinity01.doc
smpaffinity01_SOURCES = smpaffinity01/init.c
smpaffinity01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpaffinity01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpatomic01
smp_tests += smpatomic01
smp_screens += smpatomic01/smpatomic01.scn
smp_docs += smpatomic01/smpatomic01.doc
smpatomic01_SOURCES = smpatomic01/init.c
smpatomic01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpatomic01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpcache01
smp_tests += smpcache01
smp_screens += smpcache01/smpcache01.scn
smp_docs += smpcache01/smpcache01.doc
smpcache01_SOURCES = smpcache01/init.c
smpcache01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpcache01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpcapture01
smp_tests += smpcapture01
smp_screens += smpcapture01/smpcapture01.scn
smp_docs += smpcapture01/smpcapture01.doc
smpcapture01_SOURCES = smpcapture01/init.c
smpcapture01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpcapture01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpcapture02
smp_tests += smpcapture02
smp_screens += smpcapture02/smpcapture02.scn
smp_docs += smpcapture02/smpcapture02.doc
smpcapture02_SOURCES = smpcapture02/init.c
smpcapture02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpcapture02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpclock01
smp_tests += smpclock01
smp_screens += smpclock01/smpclock01.scn
smp_docs += smpclock01/smpclock01.doc
smpclock01_SOURCES = smpclock01/init.c ../support/src/spin.c
smpclock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpclock01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal01
smp_tests += smpfatal01
smp_screens += smpfatal01/smpfatal01.scn
smp_docs += smpfatal01/smpfatal01.doc
smpfatal01_SOURCES = smpfatal01/init.c
smpfatal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal02
smp_tests += smpfatal02
smp_screens += smpfatal02/smpfatal02.scn
smp_docs += smpfatal02/smpfatal02.doc
smpfatal02_SOURCES = smpfatal02/init.c
smpfatal02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal03
smp_tests += smpfatal03
smp_screens += smpfatal03/smpfatal03.scn
smp_docs += smpfatal03/smpfatal03.doc
smpfatal03_SOURCES = smpfatal03/init.c
smpfatal03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal03) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal04
smp_tests += smpfatal04
smp_screens += smpfatal04/smpfatal04.scn
smp_docs += smpfatal04/smpfatal04.doc
smpfatal04_SOURCES = smpfatal04/init.c
smpfatal04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal04) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal05
smp_tests += smpfatal05
smp_screens += smpfatal05/smpfatal05.scn
smp_docs += smpfatal05/smpfatal05.doc
smpfatal05_SOURCES = smpfatal05/init.c
smpfatal05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal05) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal06
smp_tests += smpfatal06
smp_screens += smpfatal06/smpfatal06.scn
smp_docs += smpfatal06/smpfatal06.doc
smpfatal06_SOURCES = smpfatal06/init.c
smpfatal06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal06) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal08
smp_tests += smpfatal08
smp_screens += smpfatal08/smpfatal08.scn
smp_docs += smpfatal08/smpfatal08.doc
smpfatal08_SOURCES = smpfatal08/init.c
smpfatal08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal08) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpfatal09
smp_tests += smpfatal09
smp_screens += smpfatal09/smpfatal09.scn
smp_docs += smpfatal09/smpfatal09.doc
smpfatal09_SOURCES = smpfatal09/init.c
smpfatal09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpfatal09) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpipi01
smp_tests += smpipi01
smp_screens += smpipi01/smpipi01.scn
smp_docs += smpipi01/smpipi01.doc
smpipi01_SOURCES = smpipi01/init.c
smpipi01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpipi01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpload01
smp_tests += smpload01
smp_screens += smpload01/smpload01.scn
smp_docs += smpload01/smpload01.doc
smpload01_SOURCES = smpload01/init.c
smpload01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpload01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smplock01
smp_tests += smplock01
smp_screens += smplock01/smplock01.scn
smp_docs += smplock01/smplock01.doc
smplock01_SOURCES = smplock01/init.c
smplock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smplock01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmigration01
smp_tests += smpmigration01
smp_screens += smpmigration01/smpmigration01.scn
smp_docs += smpmigration01/smpmigration01.doc
smpmigration01_SOURCES = smpmigration01/init.c
smpmigration01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmigration01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmigration02
smp_tests += smpmigration02
smp_screens += smpmigration02/smpmigration02.scn
smp_docs += smpmigration02/smpmigration02.doc
smpmigration02_SOURCES = smpmigration02/init.c
smpmigration02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmigration02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmrsp01
smp_tests += smpmrsp01
smp_screens += smpmrsp01/smpmrsp01.scn
smp_docs += smpmrsp01/smpmrsp01.doc
smpmrsp01_SOURCES = smpmrsp01/init.c
smpmrsp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmrsp01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmulticast01
smp_tests += smpmulticast01
smp_screens += smpmulticast01/smpmulticast01.scn
smp_docs += smpmulticast01/smpmulticast01.doc
smpmulticast01_SOURCES = smpmulticast01/init.c
smpmulticast01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmulticast01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmutex01
smp_tests += smpmutex01
smp_screens += smpmutex01/smpmutex01.scn
smp_docs += smpmutex01/smpmutex01.doc
smpmutex01_SOURCES = smpmutex01/init.c
smpmutex01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmutex01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpmutex02
smp_tests += smpmutex02
smp_screens += smpmutex02/smpmutex02.scn
smp_docs += smpmutex02/smpmutex02.doc
smpmutex02_SOURCES = smpmutex02/init.c
smpmutex02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpmutex02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpopenmp01
smp_tests += smpopenmp01
smp_screens += smpopenmp01/smpopenmp01.scn
smp_docs += smpopenmp01/smpopenmp01.doc
smpopenmp01_SOURCES = smpopenmp01/init.c
smpopenmp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpopenmp01) \
	$(support_includes)
smpopenmp01_CFLAGS = $(AM_CFLAGS) -fopenmp
endif
endif

if HAS_SMP
if TEST_smppsxaffinity01
smp_tests += smppsxaffinity01
smp_screens += smppsxaffinity01/smppsxaffinity01.scn
smp_docs += smppsxaffinity01/smppsxaffinity01.doc
smppsxaffinity01_SOURCES = smppsxaffinity01/init.c
smppsxaffinity01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smppsxaffinity01) $(support_includes) \
	-I$(top_srcdir)/include
endif
endif

if HAS_SMP
if TEST_smppsxaffinity02
smp_tests += smppsxaffinity02
smp_screens += smppsxaffinity02/smppsxaffinity02.scn
smp_docs += smppsxaffinity02/smppsxaffinity02.doc
smppsxaffinity02_SOURCES = smppsxaffinity02/init.c
smppsxaffinity02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smppsxaffinity02) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smppsxmutex01
smp_tests += smppsxmutex01
smp_screens += smppsxmutex01/smppsxmutex01.scn
smp_docs += smppsxmutex01/smppsxmutex01.doc
smppsxmutex01_SOURCES = smppsxmutex01/init.c
smppsxmutex01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smppsxmutex01) \
	$(support_includes)
endif
endif

if HAS_SMP
if HAS_POSIX
if TEST_smppsxsignal01
smp_tests += smppsxsignal01
smp_screens += smppsxsignal01/smppsxsignal01.scn
smp_docs += smppsxsignal01/smppsxsignal01.doc
smppsxsignal01_SOURCES = smppsxsignal01/init.c
smppsxsignal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smppsxsignal01) \
	$(support_includes)
endif
endif
endif

if HAS_SMP
if TEST_smpschedaffinity01
smp_tests += smpschedaffinity01
smp_screens += smpschedaffinity01/smpschedaffinity01.scn
smp_docs += smpschedaffinity01/smpschedaffinity01.doc
smpschedaffinity01_SOURCES = smpschedaffinity01/init.c
smpschedaffinity01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpschedaffinity01) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschedaffinity02
smp_tests += smpschedaffinity02
smp_screens += smpschedaffinity02/smpschedaffinity02.scn
smp_docs += smpschedaffinity02/smpschedaffinity02.doc
smpschedaffinity02_SOURCES = smpschedaffinity02/init.c
smpschedaffinity02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpschedaffinity02) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschedaffinity03
smp_tests += smpschedaffinity03
smp_screens += smpschedaffinity03/smpschedaffinity03.scn
smp_docs += smpschedaffinity03/smpschedaffinity03.doc
smpschedaffinity03_SOURCES = smpschedaffinity03/init.c
smpschedaffinity03_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpschedaffinity03) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschedaffinity04
smp_tests += smpschedaffinity04
smp_screens += smpschedaffinity04/smpschedaffinity04.scn
smp_docs += smpschedaffinity04/smpschedaffinity04.doc
smpschedaffinity04_SOURCES = smpschedaffinity04/init.c
smpschedaffinity04_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpschedaffinity04) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschedaffinity05
smp_tests += smpschedaffinity05
smp_screens += smpschedaffinity05/smpschedaffinity05.scn
smp_docs += smpschedaffinity05/smpschedaffinity05.doc
smpschedaffinity05_SOURCES = smpschedaffinity05/init.c
smpschedaffinity05_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpschedaffinity05) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschededf01
smp_tests += smpschededf01
smp_screens += smpschededf01/smpschededf01.scn
smp_docs += smpschededf01/smpschededf01.doc
smpschededf01_SOURCES = smpschededf01/init.c
smpschededf01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpschededf01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschededf02
smp_tests += smpschededf02
smp_screens += smpschededf02/smpschededf02.scn
smp_docs += smpschededf02/smpschededf02.doc
smpschededf02_SOURCES = smpschededf02/init.c
smpschededf02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpschededf02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschededf03
smp_tests += smpschededf03
smp_screens += smpschededf03/smpschededf03.scn
smp_docs += smpschededf03/smpschededf03.doc
smpschededf03_SOURCES = smpschededf03/init.c
smpschededf03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpschededf03) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschededf04
smp_tests += smpschededf04
smp_screens += smpschededf04/smpschededf04.scn
smp_docs += smpschededf04/smpschededf04.doc
smpschededf04_SOURCES = smpschededf04/init.c
smpschededf04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpschededf04) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpschedsem01
smp_tests += smpschedsem01
smp_screens += smpschedsem01/smpschedsem01.scn
smp_docs += smpschedsem01/smpschedsem01.doc
smpschedsem01_SOURCES = smpschedsem01/init.c
smpschedsem01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpschedsem01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler01
smp_tests += smpscheduler01
smp_screens += smpscheduler01/smpscheduler01.scn
smp_docs += smpscheduler01/smpscheduler01.doc
smpscheduler01_SOURCES = smpscheduler01/init.c
smpscheduler01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler02
smp_tests += smpscheduler02
smp_screens += smpscheduler02/smpscheduler02.scn
smp_docs += smpscheduler02/smpscheduler02.doc
smpscheduler02_SOURCES = smpscheduler02/init.c
smpscheduler02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler02) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler03
smp_tests += smpscheduler03
smp_screens += smpscheduler03/smpscheduler03.scn
smp_docs += smpscheduler03/smpscheduler03.doc
smpscheduler03_SOURCES = smpscheduler03/init.c smpscheduler03/test.c
smpscheduler03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler03) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler04
smp_tests += smpscheduler04
smp_screens += smpscheduler04/smpscheduler04.scn
smp_docs += smpscheduler04/smpscheduler04.doc
smpscheduler04_SOURCES = smpscheduler04/init.c
smpscheduler04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler04) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler05
smp_tests += smpscheduler05
smp_screens += smpscheduler05/smpscheduler05.scn
smp_docs += smpscheduler05/smpscheduler05.doc
smpscheduler05_SOURCES = smpscheduler05/init.c smpscheduler03/test.c
smpscheduler05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler05) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler06
smp_tests += smpscheduler06
smp_screens += smpscheduler06/smpscheduler06.scn
smp_docs += smpscheduler06/smpscheduler06.doc
smpscheduler06_SOURCES = smpscheduler06/init.c smpscheduler03/test.c
smpscheduler06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler06) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpscheduler07
smp_tests += smpscheduler07
smp_screens += smpscheduler07/smpscheduler07.scn
smp_docs += smpscheduler07/smpscheduler07.doc
smpscheduler07_SOURCES = smpscheduler07/init.c smpscheduler03/test.c
smpscheduler07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpscheduler07) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpsignal01
smp_tests += smpsignal01
smp_screens += smpsignal01/smpsignal01.scn
smp_docs += smpsignal01/smpsignal01.doc
smpsignal01_SOURCES = smpsignal01/init.c
smpsignal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpsignal01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpstrongapa01
smp_tests += smpstrongapa01
smp_screens += smpstrongapa01/smpstrongapa01.scn
smp_docs += smpstrongapa01/smpstrongapa01.doc
smpstrongapa01_SOURCES = smpstrongapa01/init.c
smpstrongapa01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpstrongapa01) \
	$(support_includes)
endif
endif

if HAS_SMP
if TEST_smpswitchextension01
smp_tests += smpswitchextension01
smp_screens += smpswitchextension01/smpswitchextension01.scn
smp_docs += smpswitchextension01/smpswitchextension01.doc
smpswitchextension01_SOURCES = smpswitchextension01/init.c
smpswitchextension01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpswitchextension01) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpthreadlife01
smp_tests += smpthreadlife01
smp_screens += smpthreadlife01/smpthreadlife01.scn
smp_docs += smpthreadlife01/smpthreadlife01.doc
smpthreadlife01_SOURCES = smpthreadlife01/init.c
smpthreadlife01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpthreadlife01) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpthreadpin01
smp_tests += smpthreadpin01
smp_screens += smpthreadpin01/smpthreadpin01.scn
smp_docs += smpthreadpin01/smpthreadpin01.doc
smpthreadpin01_SOURCES = smpthreadpin01/init.c
smpthreadpin01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpthreadpin01) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpunsupported01
smp_tests += smpunsupported01
smp_screens += smpunsupported01/smpunsupported01.scn
smp_docs += smpunsupported01/smpunsupported01.doc
smpunsupported01_SOURCES = smpunsupported01/init.c
smpunsupported01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_smpunsupported01) $(support_includes)
endif
endif

if HAS_SMP
if TEST_smpwakeafter01
smp_tests += smpwakeafter01
smp_screens += smpwakeafter01/smpwakeafter01.scn
smp_docs += smpwakeafter01/smpwakeafter01.doc
smpwakeafter01_SOURCES = smpwakeafter01/init.c
smpwakeafter01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_smpwakeafter01) \
	$(support_includes)
endif
endif

noinst_PROGRAMS = $(smp_tests)
