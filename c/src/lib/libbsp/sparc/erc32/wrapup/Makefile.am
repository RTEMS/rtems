##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING_DRIVER = erc32sonic
endif

BSP_PIECES = startup console clock timer gnatsupp $(NETWORKING_DRIVER)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/sparc/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/sparc/reg_win$(LIB_VARIANT).rel \
    ../../../../libcpu/sparc/syscall$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
