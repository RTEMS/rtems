##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_MP
MP_BSP_O_FILES = shmsupp
endif

BSP_FILES = startup console iic ethernet flash nvram $(MP_BSP_O_FILES)
CPU_FILES = clock timer

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(foreach piece, $(CPU_FILES), ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/$(piece).rel)
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
