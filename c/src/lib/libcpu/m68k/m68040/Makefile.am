##
## $Id$
##

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if m68040
EXTRA_PROGRAMS += fpsp.rel
CLEANFILES += fpsp.rel
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
fpsp_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
fpsp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += fpsp_g.rel
CLEANFILES += fpsp_g.rel
fpsp_g_rel_SOURCES = $(fpsp_rel_SOURCES)
fpsp_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
fpsp_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += fpsp$(LIB_VARIANT).rel

noinst_HEADERS = fpsp/fpsp.defs
endif
EXTRA_DIST = fpsp/README

include $(top_srcdir)/../../../automake/local.am
