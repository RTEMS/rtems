##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../aclocal

noinst_HEADERS = shared/include/cache.h

noinst_HEADERS += shared/src/cache_aligned_malloc.c \
shared/src/cache_manager.c


SUBDIRS = @subdirs@

DIST_SUBDIRS = \
a29k \
hppa1.1 \
i386 \
i960 \
m68k \
mips \
mips64orion \
powerpc \
sh \
sparc

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
