#
# Single Processor Testuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

sp_tests =
sp_screens =
sp_docs =
sp_libs =

support_includes = -I$(top_srcdir)/../support/include

if TEST_sp01
sp_tests += sp01
sp_screens += sp01/sp01.scn
sp_docs += sp01/sp01.doc
sp01_SOURCES = sp01/init.c sp01/task1.c sp01/system.h
sp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp01) $(support_includes)
endif

if TEST_sp02
sp_tests += sp02
sp_screens += sp02/sp02.scn
sp_docs += sp02/sp02.doc
sp02_SOURCES = sp02/init.c sp02/task1.c sp02/task2.c sp02/task3.c \
	sp02/preempt.c sp02/system.h
sp02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp02) $(support_includes)
endif

if TEST_sp03
sp_tests += sp03
sp_screens += sp03/sp03.scn
sp_docs += sp03/sp03.doc
sp03_SOURCES = sp03/init.c sp03/task1.c sp03/task2.c sp03/system.h
sp03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp03) $(support_includes)
endif

if TEST_sp04
sp_tests += sp04
sp_screens += sp04/sp04.scn
sp_docs += sp04/sp04.doc
sp04_SOURCES = sp04/init.c sp04/task1.c sp04/task2.c sp04/task3.c \
	sp04/tswitch.c sp04/system.h
sp04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp04) $(support_includes)
endif

if TEST_sp05
sp_tests += sp05
sp_screens += sp05/sp05.scn
sp_docs += sp05/sp05.doc
sp05_SOURCES = sp05/init.c sp05/task1.c sp05/task2.c sp05/task3.c \
	sp05/system.h
sp05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp05) $(support_includes)
endif

if TEST_sp06
sp_tests += sp06
sp_screens += sp06/sp06.scn
sp_docs += sp06/sp06.doc
sp06_SOURCES = sp06/init.c sp06/task1.c sp06/task2.c sp06/task3.c \
	sp06/system.h
sp06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp06) $(support_includes)
endif

if TEST_sp07
sp_tests += sp07
sp_screens += sp07/sp07.scn
sp_docs += sp07/sp07.doc
sp07_SOURCES = sp07/init.c sp07/task1.c sp07/task2.c sp07/task3.c \
	sp07/task4.c sp07/taskexit.c sp07/tcreate.c sp07/tdelete.c \
	sp07/trestart.c sp07/tstart.c sp07/system.h
sp07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp07) $(support_includes)
endif

if TEST_sp08
sp_tests += sp08
sp_screens += sp08/sp08.scn
sp_docs += sp08/sp08.doc
sp08_SOURCES = sp08/init.c
sp08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp08) $(support_includes)
endif

if TEST_sp11
sp_tests += sp11
sp_screens += sp11/sp11.scn
sp_docs += sp11/sp11.doc
sp11_SOURCES = sp11/init.c sp11/task1.c sp11/task2.c sp11/timer.c \
	sp11/system.h
sp11_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp11) $(support_includes)
endif

if TEST_sp12
sp_tests += sp12
sp_screens += sp12/sp12.scn
sp_docs += sp12/sp12.doc
sp12_SOURCES = sp12/init.c sp12/task1.c sp12/task2.c sp12/task3.c \
	sp12/task4.c sp12/task5.c sp12/pridrv.c sp12/pritask.c sp12/system.h
sp12_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp12) $(support_includes)
endif

if TEST_sp13
sp_tests += sp13
sp_screens += sp13/sp13.scn
sp_docs += sp13/sp13.doc
sp13_SOURCES = sp13/init.c sp13/fillbuff.c sp13/putbuff.c sp13/task1.c \
	sp13/task2.c sp13/task3.c sp13/system.h
sp13_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp13) $(support_includes)
endif

if TEST_sp14
sp_tests += sp14
sp_screens += sp14/sp14.scn
sp_docs += sp14/sp14.doc
sp14_SOURCES = sp14/init.c sp14/asr.c sp14/task1.c sp14/task2.c \
	sp14/system.h
sp14_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp14) $(support_includes)
endif

if TEST_sp15
sp_tests += sp15
sp_screens += sp15/sp15.scn
sp_docs += sp15/sp15.doc
sp15_SOURCES = sp15/init.c sp15/task1.c sp15/system.h
sp15_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp15) $(support_includes)
endif

if TEST_sp16
sp_tests += sp16
sp_screens += sp16/sp16.scn
sp_docs += sp16/sp16.doc
sp16_SOURCES = sp16/init.c sp16/task1.c sp16/task2.c sp16/task3.c \
	sp16/task4.c sp16/task5.c sp16/system.h
sp16_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp16) $(support_includes)
endif

if TEST_sp17
sp_tests += sp17
sp_screens += sp17/sp17.scn
sp_docs += sp17/sp17.doc
sp17_SOURCES = sp17/init.c sp17/asr.c sp17/task1.c sp17/task2.c \
	sp17/system.h
sp17_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp17) $(support_includes)
endif

if TEST_sp18
sp_tests += sp18
sp_screens += sp18/sp18.scn
sp_docs += sp18/sp18.doc
sp18_SOURCES = sp18/init.c
sp18_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp18) $(support_includes)
endif

if TEST_sp19
sp_tests += sp19
sp_screens += sp19/sp19.scn
sp_docs += sp19/sp19.doc
sp19_SOURCES = sp19/init.c sp19/first.c sp19/fptask.c sp19/task1.c \
	sp19/system.h sp19/fptest.h sp19/inttest.h
sp19_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp19) $(support_includes)
endif

if TEST_sp20
sp_tests += sp20
sp_screens += sp20/sp20.scn
sp_docs += sp20/sp20.doc
sp20_SOURCES = sp20/init.c sp20/getall.c sp20/task1.c sp20/system.h
sp20_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp20) $(support_includes)
endif

if TEST_sp2038
sp_tests += sp2038
sp_screens += sp2038/sp2038.scn
sp_docs += sp2038/sp2038.doc
sp2038_SOURCES = sp2038/init.c
sp2038_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp2038) \
	$(support_includes)
endif

if TEST_sp21
sp_tests += sp21
sp_screens += sp21/sp21.scn
sp_docs += sp21/sp21.doc
sp21_SOURCES = sp21/init.c
sp21_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp21) $(support_includes) \
	-Wno-deprecated-declarations
endif

if TEST_sp22
sp_tests += sp22
sp_screens += sp22/sp22.scn
sp_docs += sp22/sp22.doc
sp22_SOURCES = sp22/init.c sp22/prtime.c sp22/delay.c sp22/task1.c \
	sp22/system.h
sp22_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp22) $(support_includes)
endif

if TEST_sp23
sp_tests += sp23
sp_screens += sp23/sp23.scn
sp_docs += sp23/sp23.doc
sp23_SOURCES = sp23/init.c sp23/task1.c sp23/system.h
sp23_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp23) $(support_includes)
endif

if TEST_sp24
sp_tests += sp24
sp_screens += sp24/sp24.scn
sp_docs += sp24/sp24.doc
sp24_SOURCES = sp24/init.c sp24/resume.c sp24/task1.c sp24/system.h
sp24_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp24) $(support_includes)
endif

if TEST_sp25
sp_tests += sp25
sp_screens += sp25/sp25.scn
sp_docs += sp25/sp25.doc
sp25_SOURCES = sp25/init.c sp25/task1.c sp25/system.h
sp25_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp25) $(support_includes)
endif

if TEST_sp26
sp_tests += sp26
sp_screens += sp26/sp26.scn
sp_docs += sp26/sp26.doc
sp26_SOURCES = sp26/init.c sp26/task1.c sp26/system.h
sp26_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp26) $(support_includes)
endif

if TEST_sp27
sp_tests += sp27
sp_screens += sp27/sp27.scn
sp_docs += sp27/sp27.doc
sp27_SOURCES = sp27/init.c
sp27_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp27) $(support_includes)
endif

if TEST_sp27a
sp_tests += sp27a
sp_screens += sp27a/sp27a.scn
sp_docs += sp27a/sp27a.doc
sp27a_SOURCES = sp27a/init.c
sp27a_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp27a) \
	$(support_includes)
endif

if NO_SMP
if TEST_sp29
sp_tests += sp29
sp_screens += sp29/sp29.scn
sp_docs += sp29/sp29.doc
sp29_SOURCES = sp29/init.c
sp29_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp29) $(support_includes)
endif
endif

if TEST_sp30
sp_tests += sp30
sp_screens += sp30/sp30.scn
sp_docs += sp30/sp30.doc
sp30_SOURCES = sp30/init.c sp30/resume.c sp30/task1.c sp30/system.h
sp30_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp30) $(support_includes)
endif

if TEST_sp31
sp_tests += sp31
sp_screens += sp31/sp31.scn
sp_docs += sp31/sp31.doc
sp31_SOURCES = sp31/delay.c sp31/init.c sp31/prtime.c sp31/task1.c \
	sp31/system.h
sp31_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp31) $(support_includes)
endif

if TEST_sp32
sp_tests += sp32
sp_screens += sp32/sp32.scn
sp_docs += sp32/sp32.doc
sp32_SOURCES = sp32/init.c
sp32_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp32) $(support_includes)
endif

if TEST_sp33
sp_tests += sp33
sp_screens += sp33/sp33.scn
sp_docs += sp33/sp33.doc
sp33_SOURCES = sp33/init.c
sp33_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp33) $(support_includes)
endif

if TEST_sp34
sp_tests += sp34
sp_screens += sp34/sp34.scn
sp_docs += sp34/sp34.doc
sp34_SOURCES = sp34/changepri.c
sp34_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp34) $(support_includes)
endif

if TEST_sp35
sp_tests += sp35
sp_screens += sp35/sp35.scn
sp_docs += sp35/sp35.doc
sp35_SOURCES = sp35/priinv.c
sp35_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp35) $(support_includes)
endif

if TEST_sp37
sp_tests += sp37
sp_screens += sp37/sp37.scn
sp_docs += sp37/sp37.doc
sp37_SOURCES = sp37/init.c sp37/system.h
sp37_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp37) $(support_includes)
endif

if TEST_sp38
sp_tests += sp38
sp_screens += sp38/sp38.scn
sp_docs += sp38/sp38.doc
sp38_SOURCES = sp38/init.c sp38/system.h
sp38_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp38) $(support_includes)
endif

if TEST_sp40
sp_tests += sp40
sp_screens += sp40/sp40.scn
sp_docs += sp40/sp40.doc
sp40_SOURCES = sp40/init.c
sp40_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp40) $(support_includes)
endif

if TEST_sp41
sp_tests += sp41
sp_screens += sp41/sp41.scn
sp_docs += sp41/sp41.doc
sp41_SOURCES = sp41/init.c
sp41_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp41) $(support_includes)
endif

if TEST_sp42
sp_tests += sp42
sp_screens += sp42/sp42.scn
sp_docs += sp42/sp42.doc
sp42_SOURCES = sp42/init.c
sp42_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp42) $(support_includes)
endif

if TEST_sp43
sp_tests += sp43
sp_screens += sp43/sp43.scn
sp_docs += sp43/sp43.doc
sp43_SOURCES = sp43/init.c sp43/system.h
sp43_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp43) $(support_includes)
endif

if TEST_sp44
sp_tests += sp44
sp_screens += sp44/sp44.scn
sp_docs += sp44/sp44.doc
sp44_SOURCES = sp44/init.c
sp44_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp44) $(support_includes)
endif

if TEST_sp45
sp_tests += sp45
sp_screens += sp45/sp45.scn
sp_docs += sp45/sp45.doc
sp45_SOURCES = sp45/init.c
sp45_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp45) $(support_includes)
endif

if TEST_sp46
sp_tests += sp46
sp_screens += sp46/sp46.scn
sp_docs += sp46/sp46.doc
sp46_SOURCES = sp46/init.c
sp46_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp46) $(support_includes)
endif

if TEST_sp47
sp_tests += sp47
sp_screens += sp47/sp47.scn
sp_docs += sp47/sp47.doc
sp47_SOURCES = sp47/init.c
sp47_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp47) $(support_includes)
endif

if TEST_sp48
sp_tests += sp48
sp_screens += sp48/sp48.scn
sp_docs += sp48/sp48.doc
sp48_SOURCES = sp48/init.c
sp48_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp48) $(support_includes)
endif

if TEST_sp49
sp_tests += sp49
sp_screens += sp49/sp49.scn
sp_docs += sp49/sp49.doc
sp49_SOURCES = sp49/init.c
sp49_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp49) $(support_includes)
endif

if TEST_sp50
sp_tests += sp50
sp_screens += sp50/sp50.scn
sp_docs += sp50/sp50.doc
sp50_SOURCES = sp50/init.c
sp50_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp50) $(support_includes)
endif

if TEST_sp51
sp_tests += sp51
sp_screens += sp51/sp51.scn
sp_docs += sp51/sp51.doc
sp51_SOURCES = sp51/init.c
sp51_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp51) $(support_includes)
endif

if TEST_sp52
sp_tests += sp52
sp_screens += sp52/sp52.scn
sp_docs += sp52/sp52.doc
sp52_SOURCES = sp52/init.c
sp52_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp52) $(support_includes)
endif

if TEST_sp53
sp_tests += sp53
sp_screens += sp53/sp53.scn
sp_docs += sp53/sp53.doc
sp53_SOURCES = sp53/init.c
sp53_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp53) $(support_includes)
endif

if TEST_sp54
sp_tests += sp54
sp_screens += sp54/sp54.scn
sp_docs += sp54/sp54.doc
sp54_SOURCES = sp54/init.c
sp54_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp54) $(support_includes)
endif

if TEST_sp55
sp_tests += sp55
sp_screens += sp55/sp55.scn
sp_docs += sp55/sp55.doc
sp55_SOURCES = sp55/init.c
sp55_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp55) $(support_includes)
endif

if TEST_sp56
sp_tests += sp56
sp_screens += sp56/sp56.scn
sp_docs += sp56/sp56.doc
sp56_SOURCES = sp56/init.c
sp56_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp56) $(support_includes)
endif

if TEST_sp57
sp_tests += sp57
sp_screens += sp57/sp57.scn
sp_docs += sp57/sp57.doc
sp57_SOURCES = sp57/init.c
sp57_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp57) $(support_includes)
endif

if TEST_sp58
sp_tests += sp58
sp_screens += sp58/sp58.scn
sp_docs += sp58/sp58.doc
sp58_SOURCES = sp58/init.c
sp58_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp58) $(support_includes)
endif

if TEST_sp59
sp_tests += sp59
sp_screens += sp59/sp59.scn
sp_docs += sp59/sp59.doc
sp59_SOURCES = sp59/init.c
sp59_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp59) $(support_includes)
endif

if TEST_sp60
sp_tests += sp60
sp_screens += sp60/sp60.scn
sp_docs += sp60/sp60.doc
sp60_SOURCES = sp60/init.c
sp60_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp60) $(support_includes)
endif

if TEST_sp62
sp_tests += sp62
sp_screens += sp62/sp62.scn
sp_docs += sp62/sp62.doc
sp62_SOURCES = sp62/init.c
sp62_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp62) $(support_includes)
endif

if TEST_sp63
sp_tests += sp63
sp_screens += sp63/sp63.scn
sp_docs += sp63/sp63.doc
sp63_SOURCES = sp63/init.c
sp63_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp63) $(support_includes)
endif

if TEST_sp64
sp_tests += sp64
sp_screens += sp64/sp64.scn
sp_docs += sp64/sp64.doc
sp64_SOURCES = sp64/init.c
sp64_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp64) $(support_includes) \
	-I$(top_srcdir)/include
endif

if TEST_sp65
sp_tests += sp65
sp_screens += sp65/sp65.scn
sp_docs += sp65/sp65.doc
sp65_SOURCES = sp65/init.c
sp65_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp65) $(support_includes) \
	-I$(top_srcdir)/include
endif

if TEST_sp66
sp_tests += sp66
sp_screens += sp66/sp66.scn
sp_docs += sp66/sp66.doc
sp66_SOURCES = sp66/init.c
sp66_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp66) $(support_includes) \
	-I$(top_srcdir)/include
endif

if TEST_sp67
sp_tests += sp67
sp_screens += sp67/sp67.scn
sp_docs += sp67/sp67.doc
sp67_SOURCES = sp67/init.c
sp67_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp67) $(support_includes)
endif

if TEST_sp68
sp_tests += sp68
sp_screens += sp68/sp68.scn
sp_docs += sp68/sp68.doc
sp68_SOURCES = sp68/init.c
sp68_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp68) $(support_includes)
endif

if TEST_sp69
sp_tests += sp69
sp_screens += sp69/sp69.scn
sp_docs += sp69/sp69.doc
sp69_SOURCES = sp69/init.c ../support/src/spin.c
sp69_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp69) $(support_includes)
endif

if TEST_sp70
sp_tests += sp70
sp_screens += sp70/sp70.scn
sp_docs += sp70/sp70.doc
sp70_SOURCES = sp70/init.c
sp70_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp70) $(support_includes)
endif

if TEST_sp71
sp_tests += sp71
sp_screens += sp71/sp71.scn
sp_docs += sp71/sp71.doc
sp71_SOURCES = sp71/init.c
sp71_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp71) $(support_includes)
endif

if TEST_sp72
sp_tests += sp72
sp_screens += sp72/sp72.scn
sp_docs += sp72/sp72.doc
sp72_SOURCES = sp72/init.c
sp72_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp72) $(support_includes)
endif

if TEST_sp73
sp_tests += sp73
sp_screens += sp73/sp73.scn
sp_docs += sp73/sp73.doc
sp73_SOURCES = sp73/init.c
sp73_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp73) $(support_includes)
endif

if TEST_sp74
sp_tests += sp74
sp_screens += sp74/sp74.scn
sp_docs += sp74/sp74.doc
sp74_SOURCES = sp74/init.c
sp74_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp74) $(support_includes)
endif

if TEST_sp75
sp_tests += sp75
sp_screens += sp75/sp75.scn
sp_docs += sp75/sp75.doc
sp75_SOURCES = sp75/init.c
sp75_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp75) $(support_includes)
endif

if TEST_sp76
sp_tests += sp76
sp_screens += sp76/sp76.scn
sp_docs += sp76/sp76.doc
sp76_SOURCES = sp76/init.c
sp76_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp76) $(support_includes)
endif

if TEST_sp77
sp_tests += sp77
sp_screens += sp77/sp77.scn
sp_docs += sp77/sp77.doc
sp77_SOURCES = sp77/init.c
sp77_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sp77) $(support_includes)
endif

if TEST_spassoc01
sp_tests += spassoc01
sp_screens += spassoc01/spassoc01.scn
sp_docs += spassoc01/spassoc01.doc
spassoc01_SOURCES = spassoc01/init.c
spassoc01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spassoc01) \
	$(support_includes)
endif

if TEST_spatomic01
sp_tests += spatomic01
sp_screens += spatomic01/spatomic01.scn
sp_docs += spatomic01/spatomic01.doc
spatomic01_SOURCES = spatomic01/init.c
spatomic01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spatomic01) \
	$(support_includes)
endif

if TEST_spcache01
sp_tests += spcache01
sp_screens += spcache01/spcache01.scn
sp_docs += spcache01/spcache01.doc
spcache01_SOURCES = spcache01/init.c
spcache01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcache01) \
	$(support_includes)
endif

if TEST_spcbssched01
sp_tests += spcbssched01
sp_screens += spcbssched01/spcbssched01.scn
sp_docs += spcbssched01/spcbssched01.doc
spcbssched01_SOURCES = spcbssched01/init.c spcbssched01/task1.c \
	spcbssched01/system.h
spcbssched01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcbssched01) \
	$(support_includes)
endif

if TEST_spcbssched02
sp_tests += spcbssched02
sp_screens += spcbssched02/spcbssched02.scn
sp_docs += spcbssched02/spcbssched02.doc
spcbssched02_SOURCES = spcbssched02/init.c \
	spcbssched02/task_periodic.c spcbssched02/system.h
spcbssched02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcbssched02) \
	$(support_includes)
endif

if TEST_spcbssched03
sp_tests += spcbssched03
sp_screens += spcbssched03/spcbssched03.scn
sp_docs += spcbssched03/spcbssched03.doc
spcbssched03_SOURCES = spcbssched03/init.c \
	spcbssched03/tasks_periodic.c spcbssched03/tasks_aperiodic.c \
	spcbssched03/system.h spcbssched03/cbsparams.h
spcbssched03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcbssched03) \
	$(support_includes)
endif

if TEST_spchain
sp_tests += spchain
sp_screens += spchain/spchain.scn
sp_docs += spchain/spchain.doc
spchain_SOURCES = spchain/init.c
spchain_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spchain) \
	$(support_includes)
endif

if TEST_spclock_err01
sp_tests += spclock_err01
sp_screens += spclock_err01/spclock_err01.scn
sp_docs += spclock_err01/spclock_err01.doc
spclock_err01_SOURCES = spclock_err01/init.c spclock_err01/delay.c \
	spclock_err01/system.h
spclock_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spclock_err01) \
	$(support_includes)
endif

if TEST_spclock_err02
sp_tests += spclock_err02
sp_screens += spclock_err02/spclock_err02.scn
sp_docs += spclock_err02/spclock_err02.doc
spclock_err02_SOURCES = spclock_err02/init.c
spclock_err02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spclock_err02) \
	$(support_includes)
endif

if TEST_spclock_todhook01
sp_tests += spclock_todhook01
sp_screens += spclock_todhook01/spclock_todhook01.scn
sp_docs += spclock_todhook01/spclock_todhook01.doc
spclock_todhook01_SOURCES = spclock_todhook01/init.c
spclock_todhook01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spclock_todhook01) \
	$(support_includes)
endif

if TEST_spconfig01
sp_tests += spconfig01
sp_screens += spconfig01/spconfig01.scn
sp_docs += spconfig01/spconfig01.doc
spconfig01_SOURCES = spconfig01/init.c
spconfig01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spconfig01) \
	$(support_includes)
endif

if TEST_spconfig02
sp_tests += spconfig02
sp_screens += spconfig02/spconfig02.scn
sp_docs += spconfig02/spconfig02.doc
spconfig02_SOURCES = spconfig02/init.c
spconfig02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spconfig02) \
	$(support_includes)
endif

if TEST_spconsole01
sp_tests += spconsole01
sp_screens += spconsole01/spconsole01.scn
sp_docs += spconsole01/spconsole01.doc
spconsole01_SOURCES = spconsole01/init.c
spconsole01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spconsole01) \
	$(support_includes)
spconsole01_LDADD = -lm $(LDADD)
endif

if TEST_spcontext01
sp_tests += spcontext01
sp_screens += spcontext01/spcontext01.scn
sp_docs += spcontext01/spcontext01.doc
spcontext01_SOURCES = spcontext01/init.c
spcontext01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcontext01) \
	$(support_includes)
endif

if TEST_spcoverage
sp_tests += spcoverage
sp_screens += spcoverage/spcoverage.scn
sp_docs += spcoverage/spcoverage.doc
spcoverage_SOURCES = spcoverage/init.c
spcoverage_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcoverage) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_spcpucounter01
sp_tests += spcpucounter01
sp_screens += spcpucounter01/spcpucounter01.scn
sp_docs += spcpucounter01/spcpucounter01.doc
spcpucounter01_SOURCES = spcpucounter01/init.c
spcpucounter01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcpucounter01) \
	$(support_includes)
endif

if TEST_spcpuset01
sp_tests += spcpuset01
sp_screens += spcpuset01/spcpuset01.scn
sp_docs += spcpuset01/spcpuset01.doc
spcpuset01_SOURCES = spcpuset01/test.c spcpuset01/init.c
spcpuset01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcpuset01) \
	$(support_includes)
endif

if HAS_CPLUSPLUS
if TEST_spcxx01
sp_tests += spcxx01
sp_screens += spcxx01/spcxx01.scn
sp_docs += spcxx01/spcxx01.doc
spcxx01_SOURCES = spcxx01/init.cc
spcxx01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spcxx01) \
	$(support_includes)
spcxx01_CXXFLAGS = $(AM_CXXFLAGS) -std=gnu++17
endif
endif

if TEST_spedfsched01
sp_tests += spedfsched01
sp_screens += spedfsched01/spedfsched01.scn
sp_docs += spedfsched01/spedfsched01.doc
spedfsched01_SOURCES = spedfsched01/init.c spedfsched01/task1.c \
	spedfsched01/system.h
spedfsched01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spedfsched01) \
	$(support_includes)
endif

if TEST_spedfsched02
sp_tests += spedfsched02
sp_screens += spedfsched02/spedfsched02.scn
sp_docs += spedfsched02/spedfsched02.doc
spedfsched02_SOURCES = spedfsched02/init.c spedfsched02/getall.c \
	spedfsched02/task1.c spedfsched02/system.h
spedfsched02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spedfsched02) \
	$(support_includes)
endif

if TEST_spedfsched03
sp_tests += spedfsched03
sp_screens += spedfsched03/spedfsched03.scn
sp_docs += spedfsched03/spedfsched03.doc
spedfsched03_SOURCES = spedfsched03/init.c \
	spedfsched03/tasks_periodic.c spedfsched03/tasks_aperiodic.c \
	spedfsched03/system.h spedfsched03/edfparams.h
spedfsched03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spedfsched03) \
	$(support_includes)
endif

if TEST_spedfsched04
sp_tests += spedfsched04
sp_screens += spedfsched04/spedfsched04.scn
sp_docs += spedfsched04/spedfsched04.doc
spedfsched04_SOURCES = spedfsched04/init.c ../support/src/spin.c
spedfsched04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spedfsched04) \
	$(support_includes)
endif

if TEST_sperror01
sp_tests += sperror01
sp_screens += sperror01/sperror01.scn
sp_docs += sperror01/sperror01.doc
sperror01_SOURCES = sperror01/init.c
sperror01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sperror01) \
	$(support_includes)
endif

if TEST_sperror02
sp_tests += sperror02
sp_screens += sperror02/sperror02.scn
sp_docs += sperror02/sperror02.doc
sperror02_SOURCES = sperror02/init.c
sperror02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sperror02) \
	$(support_includes)
endif

if TEST_sperror03
sp_tests += sperror03
sp_screens += sperror03/sperror03.scn
sp_docs += sperror03/sperror03.doc
sperror03_SOURCES = sperror03/init.c
sperror03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sperror03) \
	$(support_includes)
endif

if TEST_spevent_err03
sp_tests += spevent_err03
sp_screens += spevent_err03/spevent_err03.scn
sp_docs += spevent_err03/spevent_err03.doc
spevent_err03_SOURCES = spevent_err03/init.c spevent_err03/system.h
spevent_err03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spevent_err03) \
	$(support_includes)
endif

if TEST_speventsystem01
sp_tests += speventsystem01
sp_screens += speventsystem01/speventsystem01.scn
sp_docs += speventsystem01/speventsystem01.doc
speventsystem01_SOURCES = speventsystem01/init.c
speventsystem01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_speventsystem01) $(support_includes)
endif

if TEST_speventtransient01
sp_tests += speventtransient01
sp_screens += speventtransient01/speventtransient01.scn
sp_docs += speventtransient01/speventtransient01.doc
speventtransient01_SOURCES = speventtransient01/init.c
speventtransient01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_speventtransient01) $(support_includes)
endif

if TEST_spextensions01
sp_tests += spextensions01
sp_screens += spextensions01/spextensions01.scn
sp_docs += spextensions01/spextensions01.doc
spextensions01_SOURCES = spextensions01/init.c
spextensions01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spextensions01) \
	$(support_includes)
endif

if TEST_spfatal01
sp_tests += spfatal01
sp_screens += spfatal01/spfatal01.scn
sp_docs += spfatal01/spfatal01.doc
spfatal01_SOURCES = spfatal01/init.c
spfatal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal01) \
	$(support_includes)
endif

if TEST_spfatal02
sp_tests += spfatal02
sp_screens += spfatal02/spfatal02.scn
sp_docs += spfatal02/spfatal02.doc
spfatal02_SOURCES = spfatal02/init.c
spfatal02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal02) \
	$(support_includes)
endif

if TEST_spfatal03
sp_tests += spfatal03
sp_screens += spfatal03/spfatal03.scn
sp_docs += spfatal03/spfatal03.doc
spfatal03_SOURCES = spfatal03/init.c
spfatal03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal03) \
	$(support_includes)
endif

if TEST_spfatal04
sp_tests += spfatal04
sp_screens += spfatal04/spfatal04.scn
sp_docs += spfatal04/spfatal04.doc
spfatal04_SOURCES = spfatal04/init.c
spfatal04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal04) \
	$(support_includes)
endif

if TEST_spfatal05
sp_tests += spfatal05
sp_screens += spfatal05/spfatal05.scn
sp_docs += spfatal05/spfatal05.doc
spfatal05_SOURCES = spfatal05/init.c
spfatal05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal05) \
	$(support_includes)
endif

if TEST_spfatal06
sp_tests += spfatal06
sp_screens += spfatal06/spfatal06.scn
sp_docs += spfatal06/spfatal06.doc
spfatal06_SOURCES = spfatal06/init.c
spfatal06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal06) \
	$(support_includes)
endif

if TEST_spfatal09
sp_tests += spfatal09
sp_screens += spfatal09/spfatal09.scn
sp_docs += spfatal09/spfatal09.doc
spfatal09_SOURCES = spfatal09/init.c
spfatal09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal09) \
	$(support_includes)
endif

if TEST_spfatal10
sp_tests += spfatal10
sp_screens += spfatal10/spfatal10.scn
sp_docs += spfatal10/spfatal10.doc
spfatal10_SOURCES = spfatal10/init.c
spfatal10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal10) \
	$(support_includes)
endif

if TEST_spfatal11
sp_tests += spfatal11
sp_screens += spfatal11/spfatal11.scn
sp_docs += spfatal11/spfatal11.doc
spfatal11_SOURCES = spfatal11/init.c
spfatal11_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal11) \
	$(support_includes)
endif

if TEST_spfatal12
sp_tests += spfatal12
sp_screens += spfatal12/spfatal12.scn
sp_docs += spfatal12/spfatal12.doc
spfatal12_SOURCES = spfatal12/init.c
spfatal12_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal12) \
	$(support_includes)
endif

if TEST_spfatal14
sp_tests += spfatal14
sp_screens += spfatal14/spfatal14.scn
sp_docs += spfatal14/spfatal14.doc
spfatal14_SOURCES = spfatal14/init.c
spfatal14_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal14) \
	$(support_includes)
endif

if TEST_spfatal15
sp_tests += spfatal15
sp_screens += spfatal15/spfatal15.scn
sp_docs += spfatal15/spfatal15.doc
spfatal15_SOURCES = spfatal15/init.c
spfatal15_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal15) \
	$(support_includes)
endif

if TEST_spfatal24
sp_tests += spfatal24
sp_screens += spfatal24/spfatal24.scn
sp_docs += spfatal24/spfatal24.doc
spfatal24_SOURCES = spfatal24/init.c
spfatal24_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal24) \
	$(support_includes)
endif

if TEST_spfatal25
sp_tests += spfatal25
sp_screens += spfatal25/spfatal25.scn
sp_docs += spfatal25/spfatal25.doc
spfatal25_SOURCES = spfatal25/init.c
spfatal25_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal25) \
	$(support_includes)
endif

if TEST_spfatal26
sp_tests += spfatal26
sp_screens += spfatal26/spfatal26.scn
sp_docs += spfatal26/spfatal26.doc
spfatal26_SOURCES = spfatal26/init.c
spfatal26_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal26) \
	$(support_includes)
endif

if TEST_spfatal28
sp_tests += spfatal28
sp_screens += spfatal28/spfatal28.scn
sp_docs += spfatal28/spfatal28.doc
spfatal28_SOURCES = spfatal28/init.c
spfatal28_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal28) \
	$(support_includes)
endif

if TEST_spfatal29
sp_tests += spfatal29
sp_screens += spfatal29/spfatal29.scn
sp_docs += spfatal29/spfatal29.doc
spfatal29_SOURCES = spfatal29/init.c
spfatal29_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal29) \
	$(support_includes)
endif

if TEST_spfatal30
sp_tests += spfatal30
sp_screens += spfatal30/spfatal30.scn
sp_docs += spfatal30/spfatal30.doc
spfatal30_SOURCES = spfatal30/init.c
spfatal30_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal30) \
	$(support_includes)
endif

if TEST_spfatal31
sp_tests += spfatal31
sp_screens += spfatal31/spfatal31.scn
sp_docs += spfatal31/spfatal31.doc
spfatal31_SOURCES = spfatal31/init.c
spfatal31_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal31) \
	$(support_includes)
endif

if TEST_spfatal32
sp_tests += spfatal32
sp_screens += spfatal32/spfatal32.scn
sp_docs += spfatal32/spfatal32.doc
spfatal32_SOURCES = spfatal32/init.c
spfatal32_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfatal32) \
	$(support_includes)
endif

if TEST_spfifo01
sp_tests += spfifo01
sp_screens += spfifo01/spfifo01.scn
sp_docs += spfifo01/spfifo01.doc
spfifo01_SOURCES = spfifo01/init.c
spfifo01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfifo01) \
	$(support_includes)
endif

if TEST_spfifo02
sp_tests += spfifo02
sp_screens += spfifo02/spfifo02.scn
sp_docs += spfifo02/spfifo02.doc
spfifo02_SOURCES = spfifo02/init.c
spfifo02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfifo02) \
	$(support_includes)
endif

if TEST_spfifo03
sp_tests += spfifo03
sp_screens += spfifo03/spfifo03.scn
sp_docs += spfifo03/spfifo03.doc
spfifo03_SOURCES = spfifo03/init.c
spfifo03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfifo03) \
	$(support_includes)
endif

if TEST_spfifo04
sp_tests += spfifo04
sp_screens += spfifo04/spfifo04.scn
sp_docs += spfifo04/spfifo04.doc
spfifo04_SOURCES = spfifo04/init.c
spfifo04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfifo04) \
	$(support_includes)
endif

if TEST_spfifo05
sp_tests += spfifo05
sp_screens += spfifo05/spfifo05.scn
sp_docs += spfifo05/spfifo05.doc
spfifo05_SOURCES = spfifo05/init.c
spfifo05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfifo05) \
	$(support_includes)
endif

if TEST_spfreechain01
sp_tests += spfreechain01
sp_screens += spfreechain01/spfreechain01.scn
sp_docs += spfreechain01/spfreechain01.doc
spfreechain01_SOURCES = spfreechain01/init.c
spfreechain01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spfreechain01) \
	$(support_includes)
endif

if HAS_CPLUSPLUS
if TEST_spglobalcon01
sp_tests += spglobalcon01
sp_screens += spglobalcon01/spglobalcon01.scn
sp_docs += spglobalcon01/spglobalcon01.doc
spglobalcon01_SOURCES = spglobalcon01/init.cc
spglobalcon01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spglobalcon01) \
	$(support_includes)
endif
endif

if TEST_spglobalcon02
sp_tests += spglobalcon02
sp_screens += spglobalcon02/spglobalcon02.scn
sp_docs += spglobalcon02/spglobalcon02.doc
spglobalcon02_SOURCES = spglobalcon02/init.c
spglobalcon02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spglobalcon02) \
	$(support_includes)
endif

if TEST_spheapprot
sp_tests += spheapprot
sp_screens += spheapprot/spheapprot.scn
sp_docs += spheapprot/spheapprot.doc
spheapprot_SOURCES = spheapprot/init.c
spheapprot_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spheapprot) \
	$(support_includes)
endif

if TEST_spinternalerror01
sp_tests += spinternalerror01
sp_screens += spinternalerror01/spinternalerror01.scn
sp_docs += spinternalerror01/spinternalerror01.doc
spinternalerror01_SOURCES = spinternalerror01/init.c
spinternalerror01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spinternalerror01) $(support_includes)
endif

if TEST_spinternalerror02
sp_tests += spinternalerror02
sp_screens += spinternalerror02/spinternalerror02.scn
sp_docs += spinternalerror02/spinternalerror02.doc
spinternalerror02_SOURCES = spinternalerror02/init.c
spinternalerror02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spinternalerror02) $(support_includes)
endif

if TEST_spintr_err01
sp_tests += spintr_err01
sp_screens += spintr_err01/spintr_err01.scn
sp_docs += spintr_err01/spintr_err01.doc
spintr_err01_SOURCES = spintr_err01/init.c spintr_err01/isr.c \
	spintr_err01/system.h
spintr_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spintr_err01) \
	$(support_includes)
endif

if TEST_spintrcritical01
sp_tests += spintrcritical01
sp_screens += spintrcritical01/spintrcritical01.scn
sp_docs += spintrcritical01/spintrcritical01.doc
spintrcritical01_SOURCES = spintrcritical01/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical01) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical02
sp_tests += spintrcritical02
sp_screens += spintrcritical02/spintrcritical02.scn
sp_docs += spintrcritical02/spintrcritical02.doc
spintrcritical02_SOURCES = spintrcritical02/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical02) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical03
sp_tests += spintrcritical03
sp_screens += spintrcritical03/spintrcritical03.scn
sp_docs += spintrcritical03/spintrcritical03.doc
spintrcritical03_SOURCES = spintrcritical03/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical03_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical03) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical04
sp_tests += spintrcritical04
sp_screens += spintrcritical04/spintrcritical04.scn
sp_docs += spintrcritical04/spintrcritical04.doc
spintrcritical04_SOURCES = spintrcritical04/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical04_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical04) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical05
sp_tests += spintrcritical05
sp_screens += spintrcritical05/spintrcritical05.scn
sp_docs += spintrcritical05/spintrcritical05.doc
spintrcritical05_SOURCES = spintrcritical05/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical05_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical05) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical06
sp_tests += spintrcritical06
sp_screens += spintrcritical06/spintrcritical06.scn
sp_docs += spintrcritical06/spintrcritical06.doc
spintrcritical06_SOURCES = spintrcritical06/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical06_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical06) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical07
sp_tests += spintrcritical07
sp_screens += spintrcritical07/spintrcritical07.scn
sp_docs += spintrcritical07/spintrcritical07.doc
spintrcritical07_SOURCES = spintrcritical07/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical07_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical07) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical08
sp_tests += spintrcritical08
sp_screens += spintrcritical08/spintrcritical08.scn
sp_docs += spintrcritical08/spintrcritical08.doc
spintrcritical08_SOURCES = spintrcritical08/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical08_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical08) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical09
sp_tests += spintrcritical09
sp_screens += spintrcritical09/spintrcritical09.scn
sp_docs += spintrcritical09/spintrcritical09.doc
spintrcritical09_SOURCES = spintrcritical09/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical09_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical09) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical10
sp_tests += spintrcritical10
sp_screens += spintrcritical10/spintrcritical10.scn
sp_docs += spintrcritical10/spintrcritical10.doc
spintrcritical10_SOURCES = spintrcritical10/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical10_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical10) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical11
sp_tests += spintrcritical11
sp_screens += spintrcritical11/spintrcritical11.scn
sp_docs += spintrcritical11/spintrcritical11.doc
spintrcritical11_SOURCES = spintrcritical11/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical11_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical11) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical12
sp_tests += spintrcritical12
sp_screens += spintrcritical12/spintrcritical12.scn
sp_docs += spintrcritical12/spintrcritical12.doc
spintrcritical12_SOURCES = spintrcritical12/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical12_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical12) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical13
sp_tests += spintrcritical13
sp_screens += spintrcritical13/spintrcritical13.scn
sp_docs += spintrcritical13/spintrcritical13.doc
spintrcritical13_SOURCES = spintrcritical13/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical13_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical13) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical14
sp_tests += spintrcritical14
sp_screens += spintrcritical14/spintrcritical14.scn
sp_docs += spintrcritical14/spintrcritical14.doc
spintrcritical14_SOURCES = spintrcritical14/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical14_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical14) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical15
sp_tests += spintrcritical15
sp_screens += spintrcritical15/spintrcritical15.scn
sp_docs += spintrcritical15/spintrcritical15.doc
spintrcritical15_SOURCES = spintrcritical15/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical15_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical15) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical16
sp_tests += spintrcritical16
sp_screens += spintrcritical16/spintrcritical16.scn
sp_docs += spintrcritical16/spintrcritical16.doc
spintrcritical16_SOURCES = spintrcritical16/init.c \
	spintrcritical_support/intrcritical.c \
	spintrcritical_support/intrcritical.h
spintrcritical16_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical16) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical18
sp_tests += spintrcritical18
sp_screens += spintrcritical18/spintrcritical18.scn
sp_docs += spintrcritical18/spintrcritical18.doc
spintrcritical18_SOURCES = spintrcritical18/init.c \
	spintrcritical_support/intrcritical.h \
	spintrcritical_support/intrcritical.c
spintrcritical18_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical18) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical20
sp_tests += spintrcritical20
sp_screens += spintrcritical20/spintrcritical20.scn
sp_docs += spintrcritical20/spintrcritical20.doc
spintrcritical20_SOURCES = spintrcritical20/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical20_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical20) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical21
sp_tests += spintrcritical21
sp_screens += spintrcritical21/spintrcritical21.scn
sp_docs += spintrcritical21/spintrcritical21.doc
spintrcritical21_SOURCES = spintrcritical21/init.c \
	spintrcritical_support/intrcritical.c
spintrcritical21_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical21) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical22
sp_tests += spintrcritical22
sp_screens += spintrcritical22/spintrcritical22.scn
sp_docs += spintrcritical22/spintrcritical22.doc
spintrcritical22_SOURCES = spintrcritical22/init.c \
	spintrcritical_support/intrcritical.h \
	spintrcritical_support/intrcritical.c
spintrcritical22_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical22) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical23
sp_tests += spintrcritical23
sp_screens += spintrcritical23/spintrcritical23.scn
sp_docs += spintrcritical23/spintrcritical23.doc
spintrcritical23_SOURCES = spintrcritical23/init.c \
	spintrcritical_support/intrcritical.h \
	spintrcritical_support/intrcritical.c
spintrcritical23_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical23) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_spintrcritical24
sp_tests += spintrcritical24
sp_screens += spintrcritical24/spintrcritical24.scn
sp_docs += spintrcritical24/spintrcritical24.doc
spintrcritical24_SOURCES = spintrcritical24/init.c \
	spintrcritical_support/intrcritical.h \
	spintrcritical_support/intrcritical.c
spintrcritical24_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spintrcritical24) $(support_includes) \
	-I$(top_srcdir)/spintrcritical_support
endif

if TEST_splinkersets01
sp_tests += splinkersets01
sp_screens += splinkersets01/splinkersets01.scn
sp_docs += splinkersets01/splinkersets01.doc
splinkersets01_SOURCES = splinkersets01/init.c
splinkersets01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_splinkersets01) \
	$(support_includes)
splinkersets01_LDADD = libsplinkersets01.a $(LDADD)
endif

if TEST_spmkdir
sp_tests += spmkdir
sp_screens += spmkdir/spmkdir.scn
sp_docs += spmkdir/spmkdir.doc
spmkdir_SOURCES = spmkdir/init.c
spmkdir_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmkdir) \
	$(support_includes)
endif

if TEST_spmisc01
sp_tests += spmisc01
sp_screens += spmisc01/spmisc01.scn
sp_docs += spmisc01/spmisc01.doc
spmisc01_SOURCES = spmisc01/init.c
spmisc01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmisc01) \
	$(support_includes)
endif

if TEST_spmountmgr01
sp_tests += spmountmgr01
sp_screens += spmountmgr01/spmountmgr01.scn
sp_docs += spmountmgr01/spmountmgr01.doc
spmountmgr01_SOURCES = spmountmgr01/init.c
spmountmgr01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmountmgr01) \
	$(support_includes)
endif

if TEST_spmrsp01
sp_tests += spmrsp01
sp_screens += spmrsp01/spmrsp01.scn
sp_docs += spmrsp01/spmrsp01.doc
spmrsp01_SOURCES = spmrsp01/init.c
spmrsp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmrsp01) \
	$(support_includes)
endif

if TEST_spmsgq_err01
sp_tests += spmsgq_err01
sp_screens += spmsgq_err01/spmsgq_err01.scn
sp_docs += spmsgq_err01/spmsgq_err01.doc
spmsgq_err01_SOURCES = spmsgq_err01/init.c spmsgq_err01/system.h
spmsgq_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmsgq_err01) \
	$(support_includes)
endif

if TEST_spmsgq_err02
sp_tests += spmsgq_err02
sp_screens += spmsgq_err02/spmsgq_err02.scn
sp_docs += spmsgq_err02/spmsgq_err02.doc
spmsgq_err02_SOURCES = spmsgq_err02/init.c spmsgq_err02/task3.c \
	spmsgq_err02/system.h
spmsgq_err02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmsgq_err02) \
	$(support_includes)
endif

if TEST_spmutex01
sp_tests += spmutex01
sp_screens += spmutex01/spmutex01.scn
sp_docs += spmutex01/spmutex01.doc
spmutex01_SOURCES = spmutex01/init.c
spmutex01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spmutex01) \
	$(support_includes)
endif

if TEST_spnsext01
sp_tests += spnsext01
sp_screens += spnsext01/spnsext01.scn
sp_docs += spnsext01/spnsext01.doc
spnsext01_SOURCES = spnsext01/init.c
spnsext01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spnsext01) \
	$(support_includes)
endif

if TEST_spobjgetnext
sp_tests += spobjgetnext
sp_screens += spobjgetnext/spobjgetnext.scn
sp_docs += spobjgetnext/spobjgetnext.doc
spobjgetnext_SOURCES = spobjgetnext/init.c spobjgetnext/system.h
spobjgetnext_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spobjgetnext) \
	$(support_includes)
endif

if TEST_sppagesize
sp_tests += sppagesize
sp_screens += sppagesize/sppagesize.scn
sp_docs += sppagesize/sppagesize.doc
sppagesize_SOURCES = sppagesize/init.c
sppagesize_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sppagesize) \
	$(support_includes)
endif

if TEST_sppartition_err01
sp_tests += sppartition_err01
sp_screens += sppartition_err01/sppartition_err01.scn
sp_docs += sppartition_err01/sppartition_err01.doc
sppartition_err01_SOURCES = sppartition_err01/init.c \
	sppartition_err01/system.h
sppartition_err01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sppartition_err01) $(support_includes)
endif

if TEST_sppercpudata01
sp_tests += sppercpudata01
sp_screens += sppercpudata01/sppercpudata01.scn
sp_docs += sppercpudata01/sppercpudata01.doc
sppercpudata01_SOURCES = sppercpudata01/init.c sppercpudata01/item.c
sppercpudata01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sppercpudata01) \
	$(support_includes)
endif

if TEST_spport_err01
sp_tests += spport_err01
sp_screens += spport_err01/spport_err01.scn
sp_docs += spport_err01/spport_err01.doc
spport_err01_SOURCES = spport_err01/init.c spport_err01/system.h
spport_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spport_err01) \
	$(support_includes)
endif

if TEST_spprintk
sp_tests += spprintk
sp_screens += spprintk/spprintk.scn
sp_docs += spprintk/spprintk.doc
spprintk_SOURCES = spprintk/init.c
spprintk_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spprintk) \
	$(support_includes)
endif

if TEST_spprivenv01
sp_tests += spprivenv01
sp_screens += spprivenv01/spprivenv01.scn
sp_docs += spprivenv01/spprivenv01.doc
spprivenv01_SOURCES = spprivenv01/init.c
spprivenv01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spprivenv01) \
	$(support_includes)
endif

if TEST_spprofiling01
sp_tests += spprofiling01
sp_screens += spprofiling01/spprofiling01.scn
sp_docs += spprofiling01/spprofiling01.doc
spprofiling01_SOURCES = spprofiling01/init.c
spprofiling01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spprofiling01) \
	$(support_includes)
endif

if TEST_spqreslib
sp_tests += spqreslib
sp_screens += spqreslib/spqreslib.scn
sp_docs += spqreslib/spqreslib.doc
spqreslib_SOURCES = spqreslib/init.c spqreslib/task_periodic.c \
	spqreslib/system.h
spqreslib_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spqreslib) \
	$(support_includes)
endif

if TEST_spratemon_err01
sp_tests += spratemon_err01
sp_screens += spratemon_err01/spratemon_err01.scn
sp_docs += spratemon_err01/spratemon_err01.doc
spratemon_err01_SOURCES = spratemon_err01/init.c \
	spratemon_err01/screen10.c spratemon_err01/task4.c \
	spratemon_err01/system.h
spratemon_err01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spratemon_err01) $(support_includes)
endif

if TEST_sprbtree01
sp_tests += sprbtree01
sp_screens += sprbtree01/sprbtree01.scn
sp_docs += sprbtree01/sprbtree01.doc
sprbtree01_SOURCES = sprbtree01/init.c
sprbtree01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sprbtree01) \
	$(support_includes)
endif

if TEST_spregion_err01
sp_tests += spregion_err01
sp_screens += spregion_err01/spregion_err01.scn
sp_docs += spregion_err01/spregion_err01.doc
spregion_err01_SOURCES = spregion_err01/init.c spregion_err01/system.h
spregion_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spregion_err01) \
	$(support_includes)
endif

if TEST_sprmsched01
sp_tests += sprmsched01
sp_screens += sprmsched01/sprmsched01.scn
sp_docs += sprmsched01/sprmsched01.doc
sprmsched01_SOURCES = sprmsched01/init.c ../support/src/spin.c
sprmsched01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sprmsched01) \
	$(support_includes)
endif

if TEST_sprmsched02
sp_tests += sprmsched02
sp_screens += sprmsched02/sprmsched02.scn
sp_docs += sprmsched02/sprmsched02.doc
sprmsched02_SOURCES = sprmsched02/init.c ../support/src/spin.c
sprmsched02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sprmsched02) \
	$(support_includes)
endif

if TEST_spscheduler01
sp_tests += spscheduler01
sp_screens += spscheduler01/spscheduler01.scn
sp_docs += spscheduler01/spscheduler01.doc
spscheduler01_SOURCES = spscheduler01/init.c
spscheduler01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spscheduler01) \
	$(support_includes)
endif

if TEST_spsem01
sp_tests += spsem01
sp_screens += spsem01/spsem01.scn
sp_docs += spsem01/spsem01.doc
spsem01_SOURCES = spsem01/init.c
spsem01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsem01) \
	$(support_includes)
endif

if TEST_spsem02
sp_tests += spsem02
sp_screens += spsem02/spsem02.scn
sp_docs += spsem02/spsem02.doc
spsem02_SOURCES = spsem02/init.c
spsem02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsem02) \
	$(support_includes)
endif

if TEST_spsem03
sp_tests += spsem03
sp_screens += spsem03/spsem03.scn
sp_docs += spsem03/spsem03.doc
spsem03_SOURCES = spsem03/init.c
spsem03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsem03) \
	$(support_includes)
endif

if TEST_spsem_err01
sp_tests += spsem_err01
sp_screens += spsem_err01/spsem_err01.scn
sp_docs += spsem_err01/spsem_err01.doc
spsem_err01_SOURCES = spsem_err01/init.c spsem_err01/system.h
spsem_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsem_err01) \
	$(support_includes)
endif

if TEST_spsem_err02
sp_tests += spsem_err02
sp_screens += spsem_err02/spsem_err02.scn
sp_docs += spsem_err02/spsem_err02.doc
spsem_err02_SOURCES = spsem_err02/init.c spsem_err02/task2.c \
	spsem_err02/system.h
spsem_err02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsem_err02) \
	$(support_includes)
endif

if TEST_spsignal_err01
sp_tests += spsignal_err01
sp_screens += spsignal_err01/spsignal_err01.scn
sp_docs += spsignal_err01/spsignal_err01.doc
spsignal_err01_SOURCES = spsignal_err01/init.c spsignal_err01/isr.c \
	spsignal_err01/system.h
spsignal_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsignal_err01) \
	$(support_includes)
endif

if TEST_spsimplesched01
sp_tests += spsimplesched01
sp_screens += spsimplesched01/spsimplesched01.scn
sp_docs += spsimplesched01/spsimplesched01.doc
spsimplesched01_SOURCES = spsimplesched01/init.c
spsimplesched01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spsimplesched01) $(support_includes)
endif

if TEST_spsimplesched02
sp_tests += spsimplesched02
sp_screens += spsimplesched02/spsimplesched02.scn
sp_docs += spsimplesched02/spsimplesched02.doc
spsimplesched02_SOURCES = spsimplesched02/init.c
spsimplesched02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spsimplesched02) $(support_includes)
endif

if TEST_spsimplesched03
sp_tests += spsimplesched03
sp_screens += spsimplesched03/spsimplesched03.scn
sp_docs += spsimplesched03/spsimplesched03.doc
spsimplesched03_SOURCES = spsimplesched03/init.c
spsimplesched03_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_spsimplesched03) $(support_includes)
endif

if TEST_spsize
sp_tests += spsize
sp_screens += spsize/spsize.scn
sp_docs += spsize/spsize.doc
spsize_SOURCES = spsize/init.c spsize/getint.c spsize/size.c \
	spsize/system.h
spsize_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsize) \
	$(support_includes)
endif

if TEST_spstdthreads01
sp_tests += spstdthreads01
sp_screens += spstdthreads01/spstdthreads01.scn
sp_docs += spstdthreads01/spstdthreads01.doc
spstdthreads01_SOURCES = spstdthreads01/init.c
spstdthreads01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spstdthreads01) \
	$(support_includes)
endif

if TEST_spstkalloc
sp_tests += spstkalloc
sp_screens += spstkalloc/spstkalloc.scn
sp_docs += spstkalloc/spstkalloc.doc
spstkalloc_SOURCES = spstkalloc/init.c
spstkalloc_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spstkalloc) \
	$(support_includes)
endif

if TEST_spstkalloc02
sp_tests += spstkalloc02
sp_screens += spstkalloc02/spstkalloc02.scn
sp_docs += spstkalloc02/spstkalloc02.doc
spstkalloc02_SOURCES = spstkalloc02/init.c
spstkalloc02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spstkalloc02) \
	$(support_includes)
endif

if TEST_spsysinit01
sp_tests += spsysinit01
sp_screens += spsysinit01/spsysinit01.scn
sp_docs += spsysinit01/spsysinit01.doc
spsysinit01_SOURCES = spsysinit01/init.c
spsysinit01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsysinit01) \
	$(support_includes)
endif

if TEST_spsyslock01
sp_tests += spsyslock01
sp_screens += spsyslock01/spsyslock01.scn
sp_docs += spsyslock01/spsyslock01.doc
spsyslock01_SOURCES = spsyslock01/init.c
spsyslock01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spsyslock01) \
	$(support_includes)
endif

if TEST_sptask_err01
sp_tests += sptask_err01
sp_screens += sptask_err01/sptask_err01.scn
sp_docs += sptask_err01/sptask_err01.doc
sptask_err01_SOURCES = sptask_err01/init.c sptask_err01/task2.c \
	sptask_err01/system.h
sptask_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptask_err01) \
	$(support_includes)
endif

if TEST_sptask_err02
sp_tests += sptask_err02
sp_screens += sptask_err02/sptask_err02.scn
sp_docs += sptask_err02/sptask_err02.doc
sptask_err02_SOURCES = sptask_err02/init.c sptask_err02/system.h
sptask_err02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptask_err02) \
	$(support_includes)
endif

if TEST_sptask_err03
sp_tests += sptask_err03
sp_screens += sptask_err03/sptask_err03.scn
sp_docs += sptask_err03/sptask_err03.doc
sptask_err03_SOURCES = sptask_err03/init.c sptask_err03/system.h
sptask_err03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptask_err03) \
	$(support_includes)
endif

if TEST_sptask_err04
sp_tests += sptask_err04
sp_screens += sptask_err04/sptask_err04.scn
sp_docs += sptask_err04/sptask_err04.doc
sptask_err04_SOURCES = sptask_err04/init.c sptask_err04/task1.c \
	sptask_err04/system.h
sptask_err04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptask_err04) \
	$(support_includes)
endif

if TEST_sptasknopreempt01
sp_tests += sptasknopreempt01
sp_screens += sptasknopreempt01/sptasknopreempt01.scn
sp_docs += sptasknopreempt01/sptasknopreempt01.doc
sptasknopreempt01_SOURCES = sptasknopreempt01/init.c
sptasknopreempt01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptasknopreempt01) $(support_includes)
endif

if TEST_spthread01
sp_tests += spthread01
sp_screens += spthread01/spthread01.scn
sp_docs += spthread01/spthread01.doc
spthread01_SOURCES = spthread01/init.c
spthread01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spthread01) \
	$(support_includes)
endif

if TEST_spthreadlife01
sp_tests += spthreadlife01
sp_screens += spthreadlife01/spthreadlife01.scn
sp_docs += spthreadlife01/spthreadlife01.doc
spthreadlife01_SOURCES = spthreadlife01/init.c
spthreadlife01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spthreadlife01) \
	$(support_includes)
endif

if TEST_spthreadq01
sp_tests += spthreadq01
sp_screens += spthreadq01/spthreadq01.scn
sp_docs += spthreadq01/spthreadq01.doc
spthreadq01_SOURCES = spthreadq01/init.c
spthreadq01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spthreadq01) \
	$(support_includes)
endif

if TEST_sptimecounter01
sp_tests += sptimecounter01
sp_screens += sptimecounter01/sptimecounter01.scn
sp_docs += sptimecounter01/sptimecounter01.doc
sptimecounter01_SOURCES = sptimecounter01/init.c
sptimecounter01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptimecounter01) $(support_includes)
endif

if TEST_sptimecounter02
sp_tests += sptimecounter02
sp_screens += sptimecounter02/sptimecounter02.scn
sp_docs += sptimecounter02/sptimecounter02.doc
sptimecounter02_SOURCES = sptimecounter02/init.c ../support/src/spin.c
sptimecounter02_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptimecounter02) $(support_includes)
endif

if TEST_sptimecounter03
sp_tests += sptimecounter03
sp_screens += sptimecounter03/sptimecounter03.scn
sp_docs += sptimecounter03/sptimecounter03.doc
sptimecounter03_SOURCES = sptimecounter03/init.c
sptimecounter03_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptimecounter03) $(support_includes)
endif

if TEST_sptimecounter04
sp_tests += sptimecounter04
sp_screens += sptimecounter04/sptimecounter04.scn
sp_docs += sptimecounter04/sptimecounter04.doc
sptimecounter04_SOURCES = sptimecounter04/init.c
sptimecounter04_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptimecounter04) $(support_includes)
endif

if TEST_sptimer_err01
sp_tests += sptimer_err01
sp_screens += sptimer_err01/sptimer_err01.scn
sp_docs += sptimer_err01/sptimer_err01.doc
sptimer_err01_SOURCES = sptimer_err01/init.c
sptimer_err01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptimer_err01) \
	$(support_includes)
endif

if TEST_sptimer_err02
sp_tests += sptimer_err02
sp_screens += sptimer_err02/sptimer_err02.scn
sp_docs += sptimer_err02/sptimer_err02.doc
sptimer_err02_SOURCES = sptimer_err02/init.c
sptimer_err02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptimer_err02) \
	$(support_includes)
endif

if TEST_sptimerserver01
sp_tests += sptimerserver01
sp_screens += sptimerserver01/sptimerserver01.scn
sp_docs += sptimerserver01/sptimerserver01.doc
sptimerserver01_SOURCES = sptimerserver01/init.c
sptimerserver01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_sptimerserver01) $(support_includes)
endif

if TEST_sptimespec01
sp_tests += sptimespec01
sp_screens += sptimespec01/sptimespec01.scn
sp_docs += sptimespec01/sptimespec01.doc
sptimespec01_SOURCES = sptimespec01/init.c
sptimespec01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptimespec01) \
	$(support_includes)
endif

if TEST_sptls01
sp_tests += sptls01
sp_screens += sptls01/sptls01.scn
sp_docs += sptls01/sptls01.doc
sptls01_SOURCES = sptls01/init.c
sptls01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptls01) \
	$(support_includes)
endif

if HAS_CPLUSPLUS
if TEST_sptls02
sp_tests += sptls02
sp_screens += sptls02/sptls02.scn
sp_docs += sptls02/sptls02.doc
sptls02_SOURCES = sptls02/init.cc sptls02/var.cc
sptls02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptls02) \
	$(support_includes)
endif
endif

if TEST_sptls03
sp_tests += sptls03
sp_screens += sptls03/sptls03.scn
sp_docs += sptls03/sptls03.doc
sptls03_SOURCES = sptls03/init.c
sptls03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptls03) \
	$(support_includes)
endif

if TEST_sptls04
sp_tests += sptls04
sp_screens += sptls04/sptls04.scn
sp_docs += sptls04/sptls04.doc
sptls04_SOURCES = sptls04/init.c
sptls04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sptls04) \
	$(support_includes)
endif

if TEST_spversion01
sp_tests += spversion01
sp_screens += spversion01/spversion01.scn
sp_docs += spversion01/spversion01.doc
spversion01_SOURCES = spversion01/init.c
spversion01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spversion01) \
	$(support_includes)
endif

if TEST_spwatchdog
sp_tests += spwatchdog
sp_screens += spwatchdog/spwatchdog.scn
sp_docs += spwatchdog/spwatchdog.doc
spwatchdog_SOURCES = spwatchdog/init.c spwatchdog/task1.c \
	spwatchdog/system.h spwatchdog/prtime.c spwatchdog/delay.c
spwatchdog_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spwatchdog) \
	$(support_includes)
endif

if TEST_spwkspace
sp_tests += spwkspace
sp_screens += spwkspace/spwkspace.scn
sp_docs += spwkspace/spwkspace.doc
spwkspace_SOURCES = spwkspace/init.c
spwkspace_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spwkspace) \
	$(support_includes)
endif

if TEST_splinkersets01
sp_libs += libsplinkersets01.a
libsplinkersets01_a_SOURCES = splinkersets01/sets.c \
	splinkersets01/items.c
endif

noinst_PROGRAMS = $(sp_tests)
noinst_LIBRARIES = $(sp_libs)
