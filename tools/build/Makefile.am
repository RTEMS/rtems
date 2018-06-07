ACLOCAL_AMFLAGS = -I ../../aclocal

bin_PROGRAMS = packhex unhex

packhex_SOURCES = packhex.c
unhex_SOURCES = unhex.c compat.c

bin_SCRIPTS = install-if-change

noinst_SCRIPTS = multigen cvsignore-add.sh

noinst_SCRIPTS += doxy-filter

include $(top_srcdir)/../../automake/host.am
