#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

bin_PROGRAMS=cklength eolstrip packhex unhex

cklength_SOURCES = cklength.c
eolstrip_SOURCES = eolstrip.c
packhex_SOURCES  = packhex.c
unhex_SOURCES    = unhex.c

bin_SCRIPTS=\
install-if-change \
lock-directory \
unlock-directory \
rcs-clean 

noinst_SCRIPTS = search-id.sh

include $(top_srcdir)/../../automake/local.am
