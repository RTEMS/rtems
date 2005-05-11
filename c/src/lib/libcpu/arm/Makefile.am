##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

SUBDIRS = at91rm9200 mc9328mxl s3c2400

noinst_PROGRAMS =


# include
if shared
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = shared/include/mmu.h

## arm920
noinst_PROGRAMS += shared/arm920.rel
shared_arm920_rel_SOURCES = shared/arm920/mmu.c
shared_arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
