##
## $Id$
##

rtems_makedir = $(prefix)/make

rtems_make_DATA = main.cfg leaf.cfg

EXTRA_DIST = host.cfg.in leaf.cfg

rtems_make_Templatesdir = $(rtems_makedir)/Templates

rtems_make_Templates_DATA = Templates/Makefile.dir Templates/Makefile.leaf \
    Templates/Makefile.lib

EXTRA_DIST += compilers/gcc-no_bsp.cfg compilers/gcc-portsw.cfg \
    compilers/gcc-target-default.cfg

rtems_make_customdir = $(rtems_makedir)/custom

rtems_make_custom_DATA = custom/default.cfg

EXTRA_DIST += custom/Cygwin-posix.cfg custom/FreeBSD-posix.cfg \
    custom/HPUX9-posix.cfg custom/Linux-posix.cfg custom/Solaris-posix.cfg \
    custom/bare.cfg custom/cvme961.cfg custom/dmv152.cfg custom/dmv177.cfg \
    custom/erc32.cfg custom/erc32nfp.cfg custom/eth_comm.cfg \
    custom/gen68302.cfg custom/gen68340.cfg custom/gen68360.cfg \
    custom/gen68360_040.cfg custom/gensh1.cfg custom/helas403.cfg \
    custom/i386ex.cfg custom/idp.cfg custom/jmr3904.cfg custom/leon1.cfg \
    custom/leon2.cfg custom/mcp750.cfg custom/mvme136.cfg custom/mvme147.cfg \
    custom/mvme147s.cfg custom/mvme162.cfg custom/mvme162lx.cfg \
    custom/mvme167.cfg custom/mvme2307.cfg custom/no_bsp.cfg \
    custom/ods68302.cfg custom/pc386.cfg custom/posix.cfg \
    custom/ppcn_60x.cfg custom/psim.cfg custom/score603e.cfg \
    custom/shsim.cfg custom/ts_386ex.cfg

include $(top_srcdir)/automake/host.am
