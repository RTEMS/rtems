##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING_DRIVER = erc32sonic
endif

BSP_PIECES = startup console clock timer gnatsupp $(NETWORKING_DRIVER)
# pieces to pick up out of libcpu/sparc
CPU_PIECES = cache reg_win syscall

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(foreach piece, $(CPU_PIECES), ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/*.o)
LIB = $(ARCH)/libbsp.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
