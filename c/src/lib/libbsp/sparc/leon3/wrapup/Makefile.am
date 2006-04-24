##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING_DRIVER = leon_open_eth leon_smc91111 leon_greth
endif

if HAS_MP
GENERIC_MP_REL_FILES = shmdr
endif
GENERIC_FILES = $(GENERIC_MP_REL_FILES)


if HAS_MP
BSP_MP_O_FILES = shmsupp
endif
BSP_PIECES = startup amba console clock $(BSP_MP_O_FILES) timer \
    gnatsupp $(NETWORKING_DRIVER)
# pieces to pick up out of libcpu/sparc
CPU_PIECES = cache reg_win syscall

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = \
 $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
 $(foreach piece, $(GENERIC_FILES), ../../../$(piece)/$(ARCH)/$(piece).rel) \
 $(foreach piece, $(CPU_PIECES), \
    ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am

../../../shmdr/o-optimize/shmdr.rel:
