## 
## $Id$
## 


BSP_PIECES = startup console i2c tod nvram

CPU_PIECES = clock console timer mbus

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
       $(foreach piece, $(CPU_PIECES), $(wildcard \
           ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(piece)/$(ARCH)/$(piece).rel)) \
    $(wildcard \
    ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/fpsp/$(ARCH)/fpsp.rel)
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

all: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
