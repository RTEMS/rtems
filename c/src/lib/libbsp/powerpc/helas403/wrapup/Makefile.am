##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

BSP_PIECES = startup dlentry flashentry
# pieces to pick up out of libcpu/ppc
CPU_PIECES = clock timer console vectors ictrl

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel \
    $(foreach piece, $(CPU_PIECES), ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(piece)/$(ARCH)/$(piece).rel)

LIB = $(ARCH)/libbsp.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a

all-local: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
