##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = librpc
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = auth_none.c auth_unix.c authunix_prot.c bindresvport.c \
	clnt_generic.c clnt_perror.c clnt_raw.c clnt_simple.c clnt_tcp.c \
	clnt_udp.c get_myaddress.c getrpcent.c getrpcport.c netname.c \
	netnamer.c pmap_clnt.c pmap_getmaps.c pmap_getport.c pmap_prot.c \
	pmap_prot2.c pmap_rmt.c rpc_callmsg.c rpc_commondata.c \
	rpc_dtablesize.c rpc_prot.c rpcdname.c rtime.c svc.c svc_auth.c \
	svc_auth_unix.c svc_raw.c svc_run.c svc_simple.c svc_tcp.c svc_udp.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a 

#
# Add local stuff here using +=
#

AM_CPPFLAGS += '-D__P(x)=x' -D_read=read -D_write=write -D_close=close

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
