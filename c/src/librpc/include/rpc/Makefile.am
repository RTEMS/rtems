##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = auth.h rpc.h svc.h types.h clnt.h rpc_msg.h svc_auth.h xdr.h

noinst_HEADERS = $(H_FILES)

if HAS_NETWORKING
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rpc/%)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rpc/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
