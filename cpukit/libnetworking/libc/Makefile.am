##
## $Id$
##


LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = addr2ascii.c ascii2addr.c base64.c gethostbydns.c gethostbyht.c \
    gethostbynis.c gethostnamadr.c getnetbydns.c getnetbyht.c getnetbynis.c \
    getnetnamadr.c getproto.c getprotoent.c getprotoname.c getservbyname.c \
    getservbyport.c getservent.c herror.c inet_addr.c inet_lnaof.c \
    inet_makeaddr.c inet_netof.c inet_network.c inet_ntoa.c inet_ntop.c \
    inet_pton.c linkaddr.c map_v4v6.c nsap_addr.c ns_name.c ns_netint.c \
    ns_parse.c ns_print.c ns_ttl.c res_comp.c res_data.c res_debug.c \
    res_init.c res_mkquery.c res_mkupdate.c res_query.c res_send.c \
    res_stubs.c res_update.c strsep.c rcmd.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/multilib.am
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

man_MANS = addr2ascii.3 byteorder.3 ethers.3 gethostbyname.3 getnetent.3 \
    getprotoent.3 getservent.3 inet.3 iso_addr.3 linkaddr.3 ns.3 rcmd.3 \
    resolver.3

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DNOPOLL -DNOSELECT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = addr2ascii.3 addr2ascii.c ascii2addr.c base64.c byteorder.3 \
    ether_addr.c ethers.3 gethostbydns.c gethostbyht.c gethostbyname.3 \
    gethostbynis.c gethostnamadr.c gethostname.c getnetbydns.c getnetbyht.c \
    getnetbynis.c getnetent.3 getnetnamadr.c getproto.c getprotoent.3 \
    getprotoent.c getprotoname.c getservbyname.c getservbyport.c \
    getservent.3 getservent.c herror.c inet.3 inet_addr.c inet_lnaof.c \
    inet_makeaddr.c inet_net_ntop.c inet_net_pton.c inet_neta.c inet_netof.c \
    inet_network.c inet_ntoa.c inet_ntop.c inet_pton.c map_v4v6.c linkaddr.3 \
    linkaddr.c ns.3 ns_addr.c ns_name.c ns_netint.c ns_ntoa.c ns_parse.c \
    ns_print.c ns_ttl.c nsap_addr.c rcmd.3 rcmd.c recv.c res_comp.c \
    res_config.h res_data.c res_debug.c res_init.c res_mkquery.c \
    res_mkupdate.c res_query.c res_send.c res_stubs.c res_update.c \
    resolver.3 send.c strsep.c

include $(top_srcdir)/../../../../automake/local.am
