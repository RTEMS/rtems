##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING_DRIVER = leon_open_eth
endif

BSP_PIECES = startup console clock timer gnatsupp $(NETWORKING_DRIVER)
# pieces to pick up out of libcpu/sparc
CPU_PIECES = cache reg_win syscall

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(foreach piece, $(CPU_PIECES), \
    ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/*.$(OBJEXT))
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
