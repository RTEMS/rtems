##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

CPLUSPLUS_TESTS = cdtest

## base_mp is a sample multiprocessing test
MP_TESTS = base_mp

FP_TESTS = paranoia

## loopback tests a network loopback interface
NET_TESTS = loopback
NET_TESTS += pppd

SUBDIRS = hello ticker base_sp unlimited minimum fileio $(MP_TESTS) \
    $(CPLUSPLUS_TESTS) $(FP_TESTS) $(NET_TESTS)

AUTOMAKE_FILES = sample.am

EXTRA_DIST = $(AUTOMAKE_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
