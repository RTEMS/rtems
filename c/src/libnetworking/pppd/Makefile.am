##
## $Id$
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES =

if HAS_NETWORKING
include_HEADERS = rtemspppd.h rtemsdialer.h

noinst_LIBRARIES += libpppd.a
libpppd_a_SOURCES = auth.c ccp.c ccp.h chap.c chap.h chap_ms.c chap_ms.h \
    chat.c demand.c fsm.c fsm.h ipcp.c ipcp.h lcp.c lcp.h magic.c magic.h \
    options.c upap.c upap.h md4.c md4.h md5.c md5.h utils.c sys-rtems.c \
    rtemsmain.c rtemspppd.c pppd.h patchlevel.h pathnames.h
libpppd_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
endif

# FIXME: Move sources to libpppd_a_SOURCES, remove other files.
EXTRA_DIST = ipxcp.c ipxcp.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtemspppd.h: rtemspppd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemspppd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemspppd.h

$(PROJECT_INCLUDE)/rtemsdialer.h: rtemsdialer.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemsdialer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemsdialer.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
