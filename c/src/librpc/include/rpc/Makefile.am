#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_NETWORKING
H_PIECES = auth.h rpc.h svc.h types.h clnt.h rpc_msg.h svc_auth.h xdr.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rpc \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rpc/%)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rpc/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
