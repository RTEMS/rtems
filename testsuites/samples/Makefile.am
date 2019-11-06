#
# Samples
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

samples =
sample_screens =
sample_docs =

support_includes = -I$(top_srcdir)/../support/include

if HAS_MP
if TEST_base_mp
# base_mp is a sample multiprocessing test
samples += base_mp_node1
sample_screens += base_mp/base_mp-node1.scn
sample_docs += base_mp/base_mp-node1.doc
base_mp_node1_SOURCES = base_mp/init1.c base_mp/apptask.c base_mp/system.h
base_mp_node1_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_base_mp) \
	$(support_includes)
samples += base_mp_node2
sample_screens += base_mp/base_mp-node2.scn
sample_docs += base_mp/base_mp-node2.doc
base_mp_node2_SOURCES = base_mp/init2.c base_mp/apptask.c base_mp/system.h
base_mp_node2_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_base_mp) \
	$(support_includes)
endif
endif

if TEST_base_sp
samples += base_sp
sample_screens += base_sp/base_sp.scn
sample_docs += base_sp/base_sp.doc
base_sp_SOURCES = base_sp/init.c base_sp/apptask.c base_sp/system.h
base_sp_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_base_sp) \
	$(support_includes)
endif

if TEST_capture
samples += capture
sample_screens += capture/capture.scn
sample_docs += capture/capture.doc
capture_SOURCES = capture/init.c capture/test1.c capture/system.h
capture_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_capture) \
	$(support_includes)
endif

if CXXTESTS
if TEST_cdtest
samples += cdtest
sample_screens += cdtest/cdtest.scn
cdtest_SOURCES = cdtest/init.c cdtest/main.cc cdtest/system.h
cdtest_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_cdtest) \
	$(support_includes)
endif
endif

if CXXTESTS
if TEST_iostream
samples += cxx_iostream
sample_screens += iostream/iostream.scn
sample_docs += iostream/iostream.doc
cxx_iostream_SOURCES = iostream/init.cc iostream/system.h
cxx_iostream_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_cxx_iostream)
endif
endif

if TEST_fileio
samples += fileio
sample_docs += fileio/fileio.doc
fileio_SOURCES = fileio/init.c fileio/system.h
fileio_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fileio) \
	$(support_includes)
endif

if TEST_hello
samples += hello
sample_screens += hello/hello.scn
sample_docs += hello/hello.doc
hello_SOURCES = hello/init.c
hello_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_hello) \
	$(support_includes)
endif

if NETTESTS
if TEST_loopback
samples += loopback
sample_screens += loopback/loopback.scn
loopback_SOURCES = loopback/init.c
loopback_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_loopback) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
endif
endif

if TEST_minimum
samples += minimum
sample_screens += minimum/minimum.scn
sample_docs += minimum/minimum.doc
minimum_SOURCES = minimum/init.c
minimum_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_minimum)
endif

if TEST_nsecs
samples += nsecs
sample_screens += nsecs/nsecs.scn
sample_docs += nsecs/nsecs.doc
nsecs_SOURCES = nsecs/init.c nsecs/empty.c
nsecs_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_nsecs) \
	$(support_includes)
endif

if TEST_paranoia
samples += paranoia
sample_screens += paranoia/paranoia.scn
sample_docs += paranoia/paranoia.doc
paranoia_SOURCES = paranoia/init.c paranoia/paranoia.c \
	paranoia/system.h
paranoia_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_paranoia) \
	$(support_includes)
paranoia_LDADD = -lm $(LDADD)
endif

if NETTESTS
if NO_SMP
if TEST_pppd
samples += pppd
pppd_SOURCES = pppd/init.c pppd/pppdapp.c pppd/netconfig.h \
	pppd/system.h
pppd_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_pppd) $(support_includes) \
	-I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
pppd_LDADD = $(RTEMS_ROOT)cpukit/libpppd.a $(LDADD)
endif
endif
endif

if TEST_ticker
samples += ticker
sample_screens += ticker/ticker.scn
sample_docs += ticker/ticker.doc
ticker_SOURCES = ticker/init.c ticker/tasks.c ticker/system.h
ticker_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_ticker) \
	$(support_includes)
endif

if TEST_unlimited
samples += unlimited
sample_screens += unlimited/unlimited.scn
sample_docs += unlimited/unlimited.doc
unlimited_SOURCES = unlimited/init.c unlimited/test1.c \
	unlimited/test2.c unlimited/test3.c unlimited/system.h
unlimited_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_unlimited) \
	$(support_includes)
endif

noinst_PROGRAMS = $(samples)
