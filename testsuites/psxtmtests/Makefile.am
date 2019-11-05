#
# POSIX Timing Teststuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

psxtm_tests =
psxtm_screens =
psxtm_docs =

support_includes = -I$(top_srcdir)/../support/include

if TEST_psxtmbarrier01
psxtm_tests += psxtmbarrier01
psxtm_docs += psxtmbarrier01/psxtmbarrier01.doc
psxtmbarrier01_SOURCES = psxtmbarrier01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmbarrier01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmbarrier01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmbarrier02
psxtm_tests += psxtmbarrier02
psxtm_docs += psxtmbarrier02/psxtmbarrier02.doc
psxtmbarrier02_SOURCES = psxtmbarrier02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmbarrier02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmbarrier02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmbarrier03
psxtm_tests += psxtmbarrier03
psxtm_docs += psxtmbarrier03/psxtmbarrier03.doc
psxtmbarrier03_SOURCES = psxtmbarrier03/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmbarrier03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmbarrier03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmbarrier04
psxtm_tests += psxtmbarrier04
psxtm_docs += psxtmbarrier04/psxtmbarrier04.doc
psxtmbarrier04_SOURCES = psxtmbarrier04/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmbarrier04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmbarrier04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmbarrierattr01
psxtm_tests += psxtmbarrierattr01
psxtm_docs += psxtmbarrierattr01/psxtmbarrierattr01.doc
psxtmbarrierattr01_SOURCES = psxtmbarrierattr01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmbarrierattr01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmbarrierattr01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcleanup01
psxtm_tests += psxtmcleanup01
psxtm_docs += psxtmcleanup01/psxtmcleanup01.doc
psxtmcleanup01_SOURCES = psxtmcleanup01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcleanup01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmcleanup01) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmclocknanosleep01
psxtm_tests += psxtmclocknanosleep01
psxtm_docs += psxtmclocknanosleep01/psxtmclocknanosleep01.doc
psxtmclocknanosleep01_SOURCES = psxtmclocknanosleep01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmclocknanosleep01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmclocknanosleep01) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmclocknanosleep02
psxtm_tests += psxtmclocknanosleep02
psxtm_docs += psxtmclocknanosleep02/psxtmclocknanosleep02.doc
psxtmclocknanosleep02_SOURCES = psxtmclocknanosleep02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmclocknanosleep02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmclocknanosleep02) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmclocknanosleep03
psxtm_tests += psxtmclocknanosleep03
psxtm_docs += psxtmclocknanosleep03/psxtmclocknanosleep03.doc
psxtmclocknanosleep03_SOURCES = psxtmclocknanosleep03/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmclocknanosleep03_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmclocknanosleep03) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond01
psxtm_tests += psxtmcond01
psxtm_docs += psxtmcond01/psxtmcond01.doc
psxtmcond01_SOURCES = psxtmcond01/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond02
psxtm_tests += psxtmcond02
psxtm_docs += psxtmcond02/psxtmcond02.doc
psxtmcond02_SOURCES = psxtmcond02/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond03
psxtm_tests += psxtmcond03
psxtm_docs += psxtmcond03/psxtmcond03.doc
psxtmcond03_SOURCES = psxtmcond03/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond04
psxtm_tests += psxtmcond04
psxtm_docs += psxtmcond04/psxtmcond04.doc
psxtmcond04_SOURCES = psxtmcond04/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond05
psxtm_tests += psxtmcond05
psxtm_docs += psxtmcond05/psxtmcond05.doc
psxtmcond05_SOURCES = psxtmcond05/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond05) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond06
psxtm_tests += psxtmcond06
psxtm_docs += psxtmcond06/psxtmcond06.doc
psxtmcond06_SOURCES = psxtmcond06/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond06) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond07
psxtm_tests += psxtmcond07
psxtm_docs += psxtmcond07/psxtmcond07.doc
psxtmcond07_SOURCES = psxtmcond07/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond07) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond08
psxtm_tests += psxtmcond08
psxtm_docs += psxtmcond08/psxtmcond08.doc
psxtmcond08_SOURCES = psxtmcond08/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond08) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond09
psxtm_tests += psxtmcond09
psxtm_docs += psxtmcond09/psxtmcond09.doc
psxtmcond09_SOURCES = psxtmcond09/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond09) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmcond10
psxtm_tests += psxtmcond10
psxtm_docs += psxtmcond10/psxtmcond10.doc
psxtmcond10_SOURCES = psxtmcond10/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmcond10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmcond10) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmkey01
psxtm_tests += psxtmkey01
psxtm_docs += psxtmkey01/psxtmkey01.doc
psxtmkey01_SOURCES = psxtmkey01/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmkey01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmkey01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmkey02
psxtm_tests += psxtmkey02
psxtm_docs += psxtmkey02/psxtmkey02.doc
psxtmkey02_SOURCES = psxtmkey02/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmkey02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmkey02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmq01
psxtm_tests += psxtmmq01
psxtm_docs += psxtmmq01/psxtmmq01.doc
psxtmmq01_SOURCES = psxtmmq01/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmq01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmq01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex01
psxtm_tests += psxtmmutex01
psxtm_docs += psxtmmutex01/psxtmmutex01.doc
psxtmmutex01_SOURCES = psxtmmutex01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex02
psxtm_tests += psxtmmutex02
psxtm_docs += psxtmmutex02/psxtmmutex02.doc
psxtmmutex02_SOURCES = psxtmmutex02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex03
psxtm_tests += psxtmmutex03
psxtm_docs += psxtmmutex03/psxtmmutex03.doc
psxtmmutex03_SOURCES = psxtmmutex03/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex04
psxtm_tests += psxtmmutex04
psxtm_docs += psxtmmutex04/psxtmmutex04.doc
psxtmmutex04_SOURCES = psxtmmutex04/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex05
psxtm_tests += psxtmmutex05
psxtm_docs += psxtmmutex05/psxtmmutex05.doc
psxtmmutex05_SOURCES = psxtmmutex05/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex05) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex06
psxtm_tests += psxtmmutex06
psxtm_docs += psxtmmutex06/psxtmmutex06.doc
psxtmmutex06_SOURCES = psxtmmutex06/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex06) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutex07
psxtm_tests += psxtmmutex07
psxtm_docs += psxtmmutex07/psxtmmutex07.doc
psxtmmutex07_SOURCES = psxtmmutex07/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutex07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutex07) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmutexattr01
psxtm_tests += psxtmmutexattr01
psxtm_docs += psxtmmutexattr01/psxtmmutexattr01.doc
psxtmmutexattr01_SOURCES = psxtmmutexattr01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmutexattr01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmutexattr01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmnanosleep01
psxtm_tests += psxtmnanosleep01
psxtm_docs += psxtmnanosleep01/psxtmnanosleep01.doc
psxtmnanosleep01_SOURCES = psxtmnanosleep01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmnanosleep01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmnanosleep01) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmnanosleep02
psxtm_tests += psxtmnanosleep02
psxtm_docs += psxtmnanosleep02/psxtmnanosleep02.doc
psxtmnanosleep02_SOURCES = psxtmnanosleep02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmnanosleep02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_psxtmnanosleep02) $(support_includes) \
	-I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmonce01
psxtm_tests += psxtmonce01
psxtm_docs += psxtmonce01/psxtmonce01.doc
psxtmonce01_SOURCES = psxtmonce01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmonce01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmonce01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock01
psxtm_tests += psxtmrwlock01
psxtm_docs += psxtmrwlock01/psxtmrwlock01.doc
psxtmrwlock01_SOURCES = psxtmrwlock01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock02
psxtm_tests += psxtmrwlock02
psxtm_docs += psxtmrwlock02/psxtmrwlock02.doc
psxtmrwlock02_SOURCES = psxtmrwlock02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock03
psxtm_tests += psxtmrwlock03
psxtm_docs += psxtmrwlock03/psxtmrwlock03.doc
psxtmrwlock03_SOURCES = psxtmrwlock03/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock04
psxtm_tests += psxtmrwlock04
psxtm_docs += psxtmrwlock04/psxtmrwlock04.doc
psxtmrwlock04_SOURCES = psxtmrwlock04/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock05
psxtm_tests += psxtmrwlock05
psxtm_docs += psxtmrwlock05/psxtmrwlock05.doc
psxtmrwlock05_SOURCES = psxtmrwlock05/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock05) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock06
psxtm_tests += psxtmrwlock06
psxtm_docs += psxtmrwlock06/psxtmrwlock06.doc
psxtmrwlock06_SOURCES = psxtmrwlock06/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock06) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmrwlock07
psxtm_tests += psxtmrwlock07
psxtm_docs += psxtmrwlock07/psxtmrwlock07.doc
psxtmrwlock07_SOURCES = psxtmrwlock07/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmrwlock07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmrwlock07) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmqrcvblock01
psxtm_tests += psxtmmqrcvblock01
psxtm_docs += psxtmmqrcvblock01/psxtmmqrcvblock01.doc
psxtmmqrcvblock01_SOURCES = psxtmmqrcvblock01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmqrcvblock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmqrcvblock01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmmqrcvblock02
psxtm_tests += psxtmmqrcvblock02
psxtm_docs += psxtmmqrcvblock02/psxtmmqrcvblock02.doc
psxtmmqrcvblock02_SOURCES = psxtmmqrcvblock02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmmqrcvblock02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmmqrcvblock02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsem01
psxtm_tests += psxtmsem01
psxtm_docs += psxtmsem01/psxtmsem01.doc
psxtmsem01_SOURCES = psxtmsem01/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsem01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsem01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsem02
psxtm_tests += psxtmsem02
psxtm_docs += psxtmsem02/psxtmsem02.doc
psxtmsem02_SOURCES = psxtmsem02/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsem02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsem02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsem03
psxtm_tests += psxtmsem03
psxtm_docs += psxtmsem03/psxtmsem03.doc
psxtmsem03_SOURCES = psxtmsem03/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsem03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsem03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsem04
psxtm_tests += psxtmsem04
psxtm_docs += psxtmsem04/psxtmsem04.doc
psxtmsem04_SOURCES = psxtmsem04/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsem04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsem04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsem05
psxtm_tests += psxtmsem05
psxtm_docs += psxtmsem05/psxtmsem05.doc
psxtmsem05_SOURCES = psxtmsem05/init.c ../tmtests/include/timesys.h \
	../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsem05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsem05) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsleep01
psxtm_tests += psxtmsleep01
psxtm_docs += psxtmsleep01/psxtmsleep01.doc
psxtmsleep01_SOURCES = psxtmsleep01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsleep01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsleep01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmsleep02
psxtm_tests += psxtmsleep02
psxtm_docs += psxtmsleep02/psxtmsleep02.doc
psxtmsleep02_SOURCES = psxtmsleep02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmsleep02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmsleep02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread01
psxtm_tests += psxtmthread01
psxtm_docs += psxtmthread01/psxtmthread01.doc
psxtmthread01_SOURCES = psxtmthread01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread02
psxtm_tests += psxtmthread02
psxtm_docs += psxtmthread02/psxtmthread02.doc
psxtmthread02_SOURCES = psxtmthread02/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread02) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread03
psxtm_tests += psxtmthread03
psxtm_docs += psxtmthread03/psxtmthread03.doc
psxtmthread03_SOURCES = psxtmthread03/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread03) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread04
psxtm_tests += psxtmthread04
psxtm_docs += psxtmthread04/psxtmthread04.doc
psxtmthread04_SOURCES = psxtmthread04/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread04) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread05
psxtm_tests += psxtmthread05
psxtm_docs += psxtmthread05/psxtmthread05.doc
psxtmthread05_SOURCES = psxtmthread05/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread05) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthread06
psxtm_tests += psxtmthread06
psxtm_docs += psxtmthread06/psxtmthread06.doc
psxtmthread06_SOURCES = psxtmthread06/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthread06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthread06) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

if TEST_psxtmthreadattr01
psxtm_tests += psxtmthreadattr01
psxtm_docs += psxtmthreadattr01/psxtmthreadattr01.doc
psxtmthreadattr01_SOURCES = psxtmthreadattr01/init.c \
	../tmtests/include/timesys.h ../support/src/tmtests_empty_function.c \
	../support/src/tmtests_support.c
psxtmthreadattr01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_psxtmthreadattr01) \
	$(support_includes) -I$(top_srcdir)/../tmtests/include
endif

noinst_PROGRAMS = $(psxtm_tests)
