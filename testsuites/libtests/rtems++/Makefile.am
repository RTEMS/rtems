##
## $Id$
##

MANAGERS = event io msg sem signal timer rate_monotonic

if HAS_CXX
noinst_LIBRARIES = lib.a
lib_a_SOURCES = Init.cc Task1.cc Task2.cc Task3.cc System.h
endif

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = rtems++.scn
docdir = $(testsdir)
dist_doc_DATA = rtems++.doc

PGM = ${ARCH}/rtems++.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if HAS_CXX
LD_LIBS = -lrtems++
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): ${ARCH}/$(dirstamp)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)
endif

include $(top_srcdir)/../automake/local.am
