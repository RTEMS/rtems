##
## $Id$
##

BSP_FILES = startup console liblnk
CPU_FILES = clock timer

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(foreach piece, $(CPU_FILES), ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/$(piece).rel)
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
