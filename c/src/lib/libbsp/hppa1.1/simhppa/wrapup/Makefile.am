##
## $Id$
##

# pieces to pick up out of libcpu/hppa
CPU_FILES = clock milli timer

if HAS_MP
GENERIC_MP_REL_FILES = shmdr
endif
GENERIC_FILES = $(GENERIC_MP_REL_FILES)

if HAS_MP
BSP_MP_O_FILES = shmsupp
endif
BSP_FILES = startup tty $(BSP_MP_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(foreach piece, $(CPU_FILES), ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/$(piece).rel) \
    $(foreach piece, $(GENERIC_FILES), ../../../$(piece)/$(ARCH)/$(piece).rel)

LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
