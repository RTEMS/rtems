##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING = network
endif

BSP_PIECES = clock irq start startup console vectors $(NETWORKING)
# pieces to pick up out of libcpu/ppc
# CPU_PIECES = mpc8xx/clock mpc8xx/console-generic mpc8xx/cpm \
#              mpc8xx/mmu mpc8xx/timer mpc8xx/vectors

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.o) \
    $(wildcard ../../../../libcpu/powerpc/new_exception_processing/$(ARCH)/*.rel) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/mpc8260/*/$(ARCH)/*.o) 
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
