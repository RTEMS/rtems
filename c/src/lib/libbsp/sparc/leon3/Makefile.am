##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories

SHMSUPP_DIR = shmsupp

SUBDIRS = . include start startup gnatsupp amba console clock timer \
    leon_open_eth leon_smc91111 leon_greth shmsupp wrapup tools leon_greth

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = bsp_specs times

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
