##
## $Id$
##

BSP_FILES = startup clock console timer gdbsci scitab

CPU_PIECES = score

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(foreach piece, $(CPU_PIECES), \
	../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(piece)/$(ARCH)/$(piece).rel) \
    $(foreach piece, $(GENERIC_FILES), ../../../$(piece)/$(ARCH)/$(piece).rel)

LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	$(make-library)

all-local: ${ARCH} $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
