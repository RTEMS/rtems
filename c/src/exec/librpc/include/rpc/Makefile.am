##
## $Id$
##


include_rpcdir = $(includedir)/rpc

H_FILES = auth.h auth_des.h auth_unix.h clnt.h des.h des_crypt.h pmap_clnt.h \
    pmap_prot.h pmap_rmt.h rpc.h rpc_com.h rpc_msg.h svc.h svc_auth.h \
    types.h xdr.h

if HAS_NETWORKING
include_rpc_HEADERS = $(H_FILES)
PREINSTALL_FILES = $(PROJECT_INCLUDE)/rpc \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rpc/%)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rpc/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(H_FILES)

include $(top_srcdir)/../automake/local.am
