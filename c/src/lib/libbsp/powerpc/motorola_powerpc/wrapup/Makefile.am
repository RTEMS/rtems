##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

BSP_PIECES = clock console irq openpic pci residual startup vectors motorola

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.o) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.o) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/*/$(ARCH)/*.o) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/*/$(ARCH)/*.o)
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
