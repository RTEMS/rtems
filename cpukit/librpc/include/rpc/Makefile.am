##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_NETWORKING
H_FILES = auth.h rpc.h svc.h types.h clnt.h rpc_msg.h svc_auth.h xdr.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rpc \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rpc/%)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rpc/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
