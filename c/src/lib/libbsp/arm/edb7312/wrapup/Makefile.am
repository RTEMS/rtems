##
## $Id$
##

BSP_FILES = clock console startup irq timer network start

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/*/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
