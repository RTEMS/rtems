##
## $Id$
##

MANAGERS = io message dual_ported_memory event multi_processor region \
    semaphore signal rate_monotonic timer partition

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c delay.c isr.c screen01.c screen02.c screen03.c screen04.c \
    screen05.c screen06.c screen07.c screen08.c screen09.c screen10.c \
    screen11.c screen12.c screen13.c screen14.c task1.c task2.c task3.c \
    task4.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = sp09.scn
docdir = $(testsdir)
dist_doc_DATA = sp09.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/sp09.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
