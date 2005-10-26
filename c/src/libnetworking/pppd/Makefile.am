##
## $Id$
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES =

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtemspppd.h rtemsdialer.h

noinst_LIBRARIES += libpppd.a
libpppd_a_SOURCES = auth.c ccp.c ccp.h chap.c chap.h chap_ms.c chap_ms.h \
    chat.c demand.c fsm.c fsm.h ipcp.c ipcp.h lcp.c lcp.h magic.c magic.h \
    options.c upap.c upap.h md4.c md4.h md5.c md5.h utils.c sys-rtems.c \
    rtemsmain.c rtemspppd.c pppd.h patchlevel.h pathnames.h
libpppd_a_CPPFLAGS = $(AM_CPPFLAGS) -D__BSD_VISIBLE
endif

# FIXME: Move sources to libpppd_a_SOURCES, remove other files.
EXTRA_DIST = ipxcp.c ipxcp.h

all-local: $(PREINSTALL_FILES)

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
