## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

BSP_FILES = startup clock console timer

if HAS_MP
GENERIC_MP_REL_FILES = shmdr
endif
GENERIC_FILES += $(GENERIC_MP_REL_FILES)

if HAS_MP
BSP_MP_O_FILES = shmsupp
endif
BSP_FILES += $(BSP_MP_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(foreach piece, $(GENERIC_FILES), ../../../$(piece)/$(ARCH)/$(piece).rel)

LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a

all: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
