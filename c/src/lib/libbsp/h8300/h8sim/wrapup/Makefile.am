##
## $Id$
##

BSP_FILES = startup clock console

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(foreach piece, $(GENERIC_FILES), \
    ../../../$(piece)/$(ARCH)/$(piece).rel)

$(ARCH)/libbsp.a: ${OBJS}
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
