##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = auth.h auth_des.h auth_unix.h clnt.h des.h des_crypt.h pmap_clnt.h \
	pmap_prot.h pmap_rmt.h rpc.h rpc_com.h rpc_msg.h svc.h svc_auth.h \
	types.h xdr.h

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
