##
## $Id$
##

PGM = $(ARCH)/fpsp.rel

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

C_FILES = rtems_fpsp.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = fpsp.defs

S_FILES = bindec.S binstr.S bugfix.S decbin.S do_func.S gen_except.S \
    get_op.S kernel_ex.S res_func.S round.S rtems_skel.S sacos.S sasin.S \
    satan.S satanh.S scale.S scosh.S setox.S sgetem.S sint.S slog2.S slogn.S \
    smovecr.S srem_mod.S ssin.S ssinh.S stan.S stanh.S sto_res.S stwotox.S \
    tbldo.S util.S x_bsun.S x_fline.S x_operr.S x_ovfl.S x_snan.S x_store.S \
    x_unfl.S x_unimp.S x_unsupp.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

if m68040
all-local: $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = README bindec.S binstr.S bugfix.S decbin.S do_func.S fpsp.defs \
    gen_except.S get_op.S kernel_ex.S res_func.S round.S rtems_fpsp.c \
    rtems_skel.S sacos.S sasin.S satan.S satanh.S scale.S scosh.S setox.S \
    sgetem.S sint.S slog2.S slogn.S smovecr.S srem_mod.S ssin.S ssinh.S \
    stan.S stanh.S sto_res.S stwotox.S tbldo.S util.S x_bsun.S x_fline.S \
    x_operr.S x_ovfl.S x_snan.S x_store.S x_unfl.S x_unimp.S x_unsupp.S

include $(top_srcdir)/../../../automake/local.am
