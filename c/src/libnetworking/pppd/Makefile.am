##
## $Id$
##

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = auth.c ccp.c chap.c chap_ms.c chat.c demand.c fsm.c ipcp.c lcp.c \
    magic.c options.c upap.c md4.c md5.c utils.c sys-rtems.c rtemsmain.c \
    rtemspppd.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

if HAS_NETWORKING
include_HEADERS = rtemspppd.h rtemsdialer.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = auth.c ccp.c ccp.h chap.c chap.h chap_ms.c chap_ms.h chat.c \
    demand.c fsm.c fsm.h ipcp.c ipcp.h ipxcp.c ipxcp.h lcp.c lcp.h magic.c \
    magic.h md4.c md4.h md5.c md5.h options.c patchlevel.h pathnames.h \
    pppd.h rtemsdialer.h rtemsmain.c rtemspppd.c rtemspppd.h sys-rtems.c \
    upap.c upap.h utils.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtemspppd.h: rtemspppd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemspppd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemspppd.h

$(PROJECT_INCLUDE)/rtemsdialer.h: rtemsdialer.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtemsdialer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtemsdialer.h
endif

include $(top_srcdir)/automake/local.am
