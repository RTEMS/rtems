##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

CLEANFILES =
EXTRA_LIBRARIES =

if HAS_NETWORKING
include_HEADERS = rtemspppd.h rtemsdialer.h

EXTRA_LIBRARIES += libpppd.a
CLEANFILES += libpppd.a
libpppd_a_SOURCES = auth.c ccp.c chap.c chap_ms.c chat.c demand.c fsm.c \
    ipcp.c lcp.c magic.c options.c upap.c md4.c md5.c utils.c sys-rtems.c \
    rtemsmain.c rtemspppd.c
libpppd_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libpppd_g.a
CLEANFILES += libpppd_g.a
libpppd_g_a_SOURCES = $(libpppd_a_SOURCES)
libpppd_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA = libpppd$(LIB_VARIANT).a
endif

# FIXME: Move sources to libpppd_a_SOURCES, remove other files.
EXTRA_DIST = ccp.h chap.h chap_ms.h fsm.h ipcp.h ipxcp.c ipxcp.h lcp.c lcp.h \
    magic.h md4.h md5.h patchlevel.h pathnames.h pppd.h upap.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtemspppd.h: rtemspppd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemspppd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemspppd.h

$(PROJECT_INCLUDE)/rtemsdialer.h: rtemsdialer.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemsdialer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemsdialer.h
endif

CLEANFILES += $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
