##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

if HAS_MP
BSP_MP_O_FILES = shmsupp
endif
BSP_FILES = startup clock console timer $(BSP_MP_O_FILES)

# pieces to pick up out of libcpu/unix

if HAS_MP
GENERIC_MP_REL_FILES = shmdr
endif
GENERIC_FILES = $(GENERIC_MP_REL_FILES)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard \
    ../$(piece)/$(ARCH)/*.$(OBJEXT))) $(foreach piece, $(GENERIC_FILES), \
    ../../../$(piece)/$(ARCH)/$(piece).rel)
LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(LIB)

include $(top_srcdir)/../../../../automake/local.am
