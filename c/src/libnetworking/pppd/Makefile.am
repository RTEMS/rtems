##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

# renamed main.c to pppmain.c
C_FILES = auth.c cbcp.c ccp.c chap.c chap_ms.c chat.c demand.c fsm.c ipcp.c \
    ipxcp.c lcp.c magic.c options.c upap.c md4.c md5.c rtems-ppp.c \
    pppmain.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

# DEFINES += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
#     -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

EXTRA_FILES = modem_example/16550.h modem_example/README \
    modem_example/modem.c modem_example/modem.h modem_example/ppp.c \
    modem_example/ppp.h modem_example/pppcompress.c

all-local: $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README STATUS auth.c cbcp.c cbcp.h ccp.c ccp.h chap.c chap.h \
    chap_ms.c chap_ms.h chat.c demand.c fsm.c fsm.h ipcp.c ipcp.h ipxcp.c \
    ipxcp.h lcp.c lcp.h magic.c magic.h main.c md4.c md4.h md5.c md5.h \
    options.c patchlevel.h pathnames.h ppp_tty.c pppd.h rtems-ppp.c upap.c \
    upap.h pppmain.c $(EXTRA_FILES)

include $(top_srcdir)/../../../automake/local.am
