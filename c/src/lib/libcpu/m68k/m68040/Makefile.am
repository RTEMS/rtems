##
## $Id$
##

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if m68040
noinst_PROGRAMS += fpsp.rel
fpsp_rel_SOURCES = fpsp/rtems_fpsp.c fpsp/bindec.S fpsp/binstr.S \
    fpsp/bugfix.S fpsp/decbin.S fpsp/do_func.S fpsp/gen_except.S \
    fpsp/get_op.S fpsp/kernel_ex.S fpsp/res_func.S fpsp/round.S \
    fpsp/rtems_skel.S fpsp/sacos.S fpsp/sasin.S fpsp/satan.S fpsp/satanh.S \
    fpsp/scale.S fpsp/scosh.S fpsp/setox.S fpsp/sgetem.S fpsp/sint.S \
    fpsp/slog2.S fpsp/slogn.S fpsp/smovecr.S fpsp/srem_mod.S fpsp/ssin.S \
    fpsp/ssinh.S fpsp/stan.S fpsp/stanh.S fpsp/sto_res.S fpsp/stwotox.S \
    fpsp/tbldo.S fpsp/util.S fpsp/x_bsun.S fpsp/x_fline.S fpsp/x_operr.S \
    fpsp/x_ovfl.S fpsp/x_snan.S fpsp/x_store.S fpsp/x_unfl.S fpsp/x_unimp.S \
    fpsp/x_unsupp.S
fpsp_rel_CPPFLAGS = $(AM_CPPFLAGS)
fpsp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_HEADERS = fpsp/fpsp.defs
endif
EXTRA_DIST = fpsp/README

include $(top_srcdir)/../../../automake/local.am
