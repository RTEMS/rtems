##
## $Id$
##

if HAS_MP
BSP_MP_O_FILES = shmsupp
endif
BSP_FILES = startup clock console timer $(BSP_MP_O_FILES)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), ../$(piece)/$(ARCH)/*.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
