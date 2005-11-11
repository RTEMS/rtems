##
##  $Id$
##

PGM = ${ARCH}/paranoia.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c paranoia.c system.h

testsdir = $(libdir)/tests
docdir = $(testsdir)
dist_doc_DATA = paranoia.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


$(PGM): ${ARCH}/$(dirstamp)

#  -DCYGNUS   turns on more verbose msgs
AM_CPPFLAGS += -DNOSIGNAL -DCYGNUS -DNOMAIN

LD_LIBS = -lm

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
