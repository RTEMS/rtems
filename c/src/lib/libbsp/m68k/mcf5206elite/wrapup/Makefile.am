##
## $Id$
##

BSP_PIECES = startup console i2c tod nvram

CPU_PIECES = clock console timer mbus

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard \
    ../../../../libcpu/$(RTEMS_CPU)/shared/misc/$(ARCH)/*.$(OBJEXT)) \
    $(foreach piece, $(CPU_PIECES), \
    ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(piece)/$(ARCH)/$(piece).rel) \
    $(wildcard \
    ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/fpsp/$(ARCH)/fpsp.rel)
LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(LIB)

include $(top_srcdir)/../../../../automake/local.am
