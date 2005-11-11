##
##  Makefile.am,v 1.9 2002/08/11 05:18:59 ralf Exp
##


MANAGERS = io message semaphore

if LIBCHIP
PGM = ${ARCH}/fileio.exe

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h
OBJS = $(lib_a_OBJECTS)

testsdir = $(libdir)/tests
docdir = $(testsdir)
dist_doc_DATA = fileio.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


if LIBCHIP
$(PGM): ${ARCH}/$(dirstamp)

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
