##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

bin_PROGRAMS = cklength eolstrip packhex unhex

noinst_PROGRAMS = binpatch

cklength_SOURCES = cklength.c
eolstrip_SOURCES = eolstrip.c
packhex_SOURCES = packhex.c
unhex_SOURCES = unhex.c
binpatch_SOURCES = binpatch.c

bin_SCRIPTS = install-if-change lock-directory unlock-directory

noinst_SCRIPTS = search-id.sh multigen

include $(top_srcdir)/../../automake/host.am
