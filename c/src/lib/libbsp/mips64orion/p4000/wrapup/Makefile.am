##
## $Id$
##

BSP_FILES = startup console liblnk
CPU_FILES = clock timer

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(foreach piece, $(CPU_FILES), \
    ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/$(piece).rel)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
