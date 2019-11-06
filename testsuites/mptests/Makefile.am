#
# Multiple Processor Teststuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

mp_tests =
mp_screens =
mp_docs =
mp_lib =

support_includes = -I$(top_srcdir)/../support/include

if HAS_MP
if TEST_mp01
mp_tests += mp01_node1
mp_screens += mp01/mp01-node1.scn
mp_docs += mp01/mp01-node1.doc
mp01_node1_SOURCES = mp01/init1.c mp01/task1.c mp01/system.h
mp01_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp01) \
	$(support_includes)
mp_tests += mp01_node2
mp_screens += mp01/mp01-node2.scn
mp_docs += mp01/mp01-node2.doc
mp01_node2_SOURCES = mp01/init2.c mp01/task1.c mp01/system.h
mp01_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp01) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp03
mp_tests += mp03_node1
mp_screens += mp03/mp03-node1.scn
mp_docs += mp03/mp03-node1.doc
mp03_node1_SOURCES = mp03/init1.c mp03/task1.c mp03/delay.c mp03/system.h
mp03_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp03) \
	$(support_includes)
mp_tests += mp03_node2
mp_screens += mp03/mp03-node2.scn
mp_docs += mp03/mp03-node2.doc
mp03_node2_SOURCES = mp03/init2.c mp03/task1.c mp03/delay.c mp03/system.h
mp03_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp03) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp04
mp_tests += mp04_node1
mp_screens += mp04/mp04-node1.scn
mp_docs += mp04/mp04-node1.doc
mp04_node1_SOURCES = mp04/init1.c mp04/task1.c mp04/system.h
mp04_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp04) \
	$(support_includes)
mp_tests += mp04_node2
mp_screens += mp04/mp04-node2.scn
mp_docs += mp04/mp04-node2.doc
mp04_node2_SOURCES = mp04/init2.c mp04/task1.c mp04/system.h
mp04_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp04) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp05
mp_tests += mp05_node1
mp_screens += mp05/mp05-node1.scn
mp_docs += mp05/mp05-node1.doc
mp05_node1_SOURCES = mp05/init1.c mp05/task1.c mp05/asr.c mp05/system.h
mp05_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp05) \
	$(support_includes)
mp_tests += mp05_node2
mp_screens += mp05/mp05-node2.scn
mp_docs += mp05/mp05-node2.doc
mp05_node2_SOURCES = mp05/init2.c mp05/task1.c mp05/asr.c mp05/system.h
mp05_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp05) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp06
mp_tests += mp06_node1
mp_screens += mp06/mp06-node1.scn
mp_docs += mp06/mp06-node1.doc
mp06_node1_SOURCES = mp06/init1.c mp06/task1.c mp06/system.h
mp06_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp06) \
	$(support_includes)
mp_tests += mp06_node2
mp_screens += mp06/mp06-node2.scn
mp_docs += mp06/mp06-node2.doc
mp06_node2_SOURCES = mp06/init2.c mp06/task1.c mp06/system.h
mp06_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp06) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp07
mp_tests += mp07_node1
mp_screens += mp07/mp07-node1.scn
mp_docs += mp07/mp07-node1.doc
mp07_node1_SOURCES = mp07/init1.c mp07/task1.c mp07/system.h
mp07_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp07) \
	$(support_includes)
mp_tests += mp07_node2
mp_screens += mp07/mp07-node2.scn
mp_docs += mp07/mp07-node2.doc
mp07_node2_SOURCES = mp07/init2.c mp07/task1.c mp07/system.h
mp07_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp07) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp08
mp_tests += mp08_node1
mp_screens += mp08/mp08-node1.scn
mp_docs += mp08/mp08-node1.doc
mp08_node1_SOURCES = mp08/init1.c mp08/task1.c mp08/system.h
mp08_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp08) \
	$(support_includes)
mp_tests += mp08_node2
mp_screens += mp08/mp08-node2.scn
mp_docs += mp08/mp08-node2.doc
mp08_node2_SOURCES = mp08/init2.c mp08/task1.c mp08/system.h
mp08_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp08) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp09
mp_tests += mp09_node1
mp_screens += mp09/mp09-node1.scn
mp_docs += mp09/mp09-node1.doc
mp09_node1_SOURCES = mp09/init1.c mp09/task1.c mp09/recvmsg.c \
	mp09/sendmsg.c mp09/system.h
mp09_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp09) \
	$(support_includes)
mp_tests += mp09_node2
mp_screens += mp09/mp09-node2.scn
mp_docs += mp09/mp09-node2.doc
mp09_node2_SOURCES = mp09/init2.c mp09/task1.c mp09/recvmsg.c \
	mp09/sendmsg.c mp09/system.h
mp09_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp09) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp10
mp_tests += mp10_node1
mp_screens += mp10/mp10-node1.scn
mp_docs += mp10/mp10-node1.doc
mp10_node1_SOURCES = mp10/init1.c mp10/task1.c mp10/task2.c mp10/task3.c \
	mp10/system.h
mp10_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp10) \
	$(support_includes)
mp_tests += mp10_node2
mp_screens += mp10/mp10-node2.scn
mp_docs += mp10/mp10-node2.doc
mp10_node2_SOURCES = mp10/init2.c mp10/task1.c mp10/task2.c mp10/task3.c \
	mp10/system.h
mp10_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp10) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp11
mp_tests += mp11_node1
mp_screens += mp11/mp11-node1.scn
mp_docs += mp11/mp11-node1.doc
mp11_node1_SOURCES = mp11/init1.c mp11/system.h
mp11_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp11) \
	$(support_includes)
mp_tests += mp11_node2
mp_screens += mp11/mp11-node2.scn
mp_docs += mp11/mp11-node2.doc
mp11_node2_SOURCES = mp11/init2.c mp11/system.h
mp11_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp11) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp12
mp_tests += mp12_node1
mp_screens += mp12/mp12-node1.scn
mp_docs += mp12/mp12-node1.doc
mp12_node1_SOURCES = mp12/init1.c mp12/system.h
mp12_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp12) \
	$(support_includes)
mp_tests += mp12_node2
mp_screens += mp12/mp12-node2.scn
mp_docs += mp12/mp12-node2.doc
mp12_node2_SOURCES = mp12/init2.c mp12/system.h
mp12_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp12) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp13
mp_tests += mp13_node1
mp_screens += mp13/mp13-node1.scn
mp_docs += mp13/mp13-node1.doc
mp13_node1_SOURCES = mp13/init1.c mp13/task1.c mp13/task2.c mp13/system.h
mp13_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp13) \
	$(support_includes)
mp_tests += mp13_node2
mp_screens += mp13/mp13-node2.scn
mp_docs += mp13/mp13-node2.doc
mp13_node2_SOURCES = mp13/init2.c mp13/task1.c mp13/task2.c mp13/system.h
mp13_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp13) \
	$(support_includes)
endif
endif

if HAS_MP
if TEST_mp14
mp_tests += mp14_node1
mp_screens += mp14/mp14-node1.scn
mp_docs += mp14/mp14-node1.doc
mp14_node1_SOURCES = mp14/init1.c mp14/exit.c mp14/delay.c \
	mp14/evtask1.c mp14/evtmtask.c mp14/pttask1.c mp14/smtask1.c \
	mp14/msgtask1.c mp14/system.h
mp14_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp14) \
	$(support_includes)
mp_tests += mp14_node2
mp_screens += mp14/mp14-node2.scn
mp_docs += mp14/mp14-node2.doc
mp14_node2_SOURCES = mp14/init2.c mp14/exit.c mp14/delay.c \
	mp14/evtask1.c mp14/evtmtask.c mp14/pttask1.c mp14/smtask1.c \
	mp14/msgtask1.c mp14/system.h
mp14_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mp14) \
	$(support_includes)
endif
endif

noinst_PROGRAMS = $(mp_tests)
