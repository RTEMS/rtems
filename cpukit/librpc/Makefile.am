##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

if LIBRPC
include_rpcdir = $(includedir)/rpc
include_rpc_HEADERS = include/rpc/auth.h include/rpc/auth_des.h \
    include/rpc/auth_unix.h include/rpc/clnt.h include/rpc/des.h \
    include/rpc/des_crypt.h include/rpc/pmap_clnt.h include/rpc/pmap_prot.h \
    include/rpc/pmap_rmt.h include/rpc/rpc.h include/rpc/rpc_com.h \
    include/rpc/rpc_msg.h include/rpc/svc.h include/rpc/svc_auth.h \
    include/rpc/types.h include/rpc/xdr.h

man_MANS =
#endif

libxdr_C_FILES = src/xdr/xdr.c src/xdr/xdr_array.c src/xdr/xdr_float.c \
    src/xdr/xdr_mem.c src/xdr/xdr_rec.c src/xdr/xdr_reference.c \
    src/xdr/xdr_sizeof.c src/xdr/xdr_stdio.c

CLEANDIRS = src/xdr

EXTRA_DIST = $(libxdr_C_FILES)

if LIBRPC
libxdr_C_O_FILES = \
    $(libxdr_C_FILES:src/xdr/%.c=src/xdr/$(ARCH)/%.$(OBJEXT))

libxdr_OBJS = $(libxdr_C_O_FILES)

src/xdr/$(ARCH):
	@$(mkinstalldirs) $@
src/xdr/$(ARCH)/%.$(OBJEXT): src/xdr/%.c
	${COMPILE} -o $@ -c $<
src/xdr/$(ARCH)/libxdr.a: $(libxdr_OBJS)
	$(make-library)

man_MANS += src/xdr/xdr.3
all-local: src/xdr/$(ARCH) src/xdr/$(ARCH)/libxdr.a
endif

librpc_C_FILES = src/rpc/auth_none.c src/rpc/auth_unix.c \
    src/rpc/authunix_prot.c src/rpc/bindresvport.c src/rpc/clnt_generic.c \
    src/rpc/clnt_perror.c src/rpc/clnt_raw.c src/rpc/clnt_simple.c \
    src/rpc/clnt_tcp.c src/rpc/clnt_udp.c src/rpc/get_myaddress.c \
    src/rpc/getrpcent.c src/rpc/getrpcport.c src/rpc/netname.c \
    src/rpc/netnamer.c src/rpc/pmap_clnt.c src/rpc/pmap_getmaps.c \
    src/rpc/pmap_getport.c src/rpc/pmap_prot.c src/rpc/pmap_prot2.c \
    src/rpc/pmap_rmt.c src/rpc/rpc_callmsg.c src/rpc/rpc_commondata.c \
    src/rpc/rpc_dtablesize.c src/rpc/rpc_prot.c src/rpc/rpcdname.c \
    src/rpc/rtime.c src/rpc/svc.c src/rpc/svc_auth.c src/rpc/svc_auth_unix.c \
    src/rpc/svc_raw.c src/rpc/svc_run.c src/rpc/svc_simple.c \
    src/rpc/svc_tcp.c src/rpc/svc_udp.c src/rpc/rtems_portmapper.c \
    src/rpc/rtems_rpc.c
librpc_UNUSED_C_FILES = src/rpc/auth_des.c src/rpc/auth_time.c \
    src/rpc/authdes_prot.c src/rpc/clnt_unix.c src/rpc/crypt_client.c \
    src/rpc/des_crypt.c src/rpc/des_soft.c src/rpc/getpublickey.c \
    src/rpc/key_call.c src/rpc/key_prot_xdr.c src/rpc/svc_auth_des.c \
    src/rpc/svc_unix.c

if LIBRPC
librpc_C_O_FILES = \
    $(librpc_C_FILES:src/rpc/%.c=src/rpc/$(ARCH)/%.$(OBJEXT))

librpc_OBJS = $(librpc_C_O_FILES)

librpc_a_CPPFLAGS = -D_RPC_read=read -D_RPC_write=write -D_RPC_close=close \
    -D_RTEMS_RPC_INTERNAL_

src/rpc/$(ARCH):
	@$(mkinstalldirs) $@
src/rpc/$(ARCH)/%.$(OBJEXT): src/rpc/%.c
	${COMPILE} $(librpc_a_CPPFLAGS) -o $@ -c $<
src/rpc/$(ARCH)/librpc.a: $(librpc_OBJS)
	$(make-library)

man_MANS += src/rpc/bindresvport.3 src/rpc/getrpcent.3 src/rpc/publickey.3 \
    src/rpc/rpc.3 src/rpc/rpc_secure.3 src/rpc/rstat_svc.8 \
    src/rpc/des_crypt.3 src/rpc/getrpcport.3 src/rpc/publickey.5 \
    src/rpc/rpc.5 src/rpc/rstat.1 src/rpc/rtime.3

all-local: src/rpc/$(ARCH) src/rpc/$(ARCH)/librpc.a
endif

CLEANDIRS += src/rpc

EXTRA_DIST += $(librpc_C_FILES) src/rpc/DISCLAIMER $(librpc_UNUSED_C_FILES)

EXTRA_DIST += src/rpc/PSD.doc/nfs.rfc.ms src/rpc/PSD.doc/rpc.prog.ms \
    src/rpc/PSD.doc/rpc.rfc.ms src/rpc/PSD.doc/rpcgen.ms \
    src/rpc/PSD.doc/xdr.nts.ms src/rpc/PSD.doc/xdr.rfc.ms

endif

PREINSTALL_FILES =

if LIBRPC
$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rpc
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc

$(PROJECT_INCLUDE)/rpc/auth.h: include/rpc/auth.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth.h

$(PROJECT_INCLUDE)/rpc/auth_des.h: include/rpc/auth_des.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth_des.h

$(PROJECT_INCLUDE)/rpc/auth_unix.h: include/rpc/auth_unix.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth_unix.h

$(PROJECT_INCLUDE)/rpc/clnt.h: include/rpc/clnt.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/clnt.h

$(PROJECT_INCLUDE)/rpc/des.h: include/rpc/des.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/des.h

$(PROJECT_INCLUDE)/rpc/des_crypt.h: include/rpc/des_crypt.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/des_crypt.h

$(PROJECT_INCLUDE)/rpc/pmap_clnt.h: include/rpc/pmap_clnt.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_clnt.h

$(PROJECT_INCLUDE)/rpc/pmap_prot.h: include/rpc/pmap_prot.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_prot.h

$(PROJECT_INCLUDE)/rpc/pmap_rmt.h: include/rpc/pmap_rmt.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_rmt.h

$(PROJECT_INCLUDE)/rpc/rpc.h: include/rpc/rpc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc.h

$(PROJECT_INCLUDE)/rpc/rpc_com.h: include/rpc/rpc_com.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc_com.h

$(PROJECT_INCLUDE)/rpc/rpc_msg.h: include/rpc/rpc_msg.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc_msg.h

$(PROJECT_INCLUDE)/rpc/svc.h: include/rpc/svc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/svc.h

$(PROJECT_INCLUDE)/rpc/svc_auth.h: include/rpc/svc_auth.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/svc_auth.h

$(PROJECT_INCLUDE)/rpc/types.h: include/rpc/types.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/types.h

$(PROJECT_INCLUDE)/rpc/xdr.h: include/rpc/xdr.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/xdr.h

endif

include $(top_srcdir)/automake/local.am
