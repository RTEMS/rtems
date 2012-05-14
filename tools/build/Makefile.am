ACLOCAL_AMFLAGS = -I ../../aclocal

bin_PROGRAMS = cklength eolstrip packhex unhex rtems-bin2c

noinst_PROGRAMS = binpatch

cklength_SOURCES = cklength.c
eolstrip_SOURCES = eolstrip.c
packhex_SOURCES = packhex.c
unhex_SOURCES = unhex.c
binpatch_SOURCES = binpatch.c
rtems_bin2c_SOURCES = rtems-bin2c.c

bin_SCRIPTS = install-if-change

noinst_SCRIPTS = multigen cvsignore-add.sh
EXTRA_DIST = multigen cvsignore-add.sh

noinst_SCRIPTS += doxy-filter
EXTRA_DIST += doxy-filter

include $(top_srcdir)/../../automake/host.am
