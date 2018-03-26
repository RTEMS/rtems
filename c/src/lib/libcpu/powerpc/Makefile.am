ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

EXTRA_DIST += ppc403/README ppc403/vectors/README
if ppc4xx
# ppc403/clock
noinst_PROGRAMS += ppc403/clock.rel
ppc403_clock_rel_SOURCES = ppc403/clock/clock.c
ppc403_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# ppc403/console
noinst_PROGRAMS += ppc403/console.rel
if ppc405
ppc403_console_rel_SOURCES = ppc403/console/console405.c
else
ppc403_console_rel_SOURCES = ppc403/console/console.c
endif
ppc403_console_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# ppc403/tty_drv
if ppc405
noinst_PROGRAMS += ppc403/tty_drv.rel
ppc403_tty_drv_rel_SOURCES = ppc403/tty_drv/tty_drv.c ppc403/tty_drv/tty_drv.h
ppc403_tty_drv_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_tty_drv_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

endif # ppc4xx

if ppc405

## ppc4xx/include
endif # ppc405

if mpc6xx

# mpc6xx/clock
noinst_PROGRAMS += mpc6xx/clock.rel
mpc6xx_clock_rel_SOURCES = mpc6xx/clock/c_clock.c mpc6xx/clock/c_clock.h
mpc6xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc6xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc6xx/altivec
noinst_PROGRAMS += mpc6xx/altivec.rel
mpc6xx_altivec_rel_SOURCES = mpc6xx/altivec/vec_sup.c mpc6xx/altivec/vec_sup_asm.S
mpc6xx_altivec_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc6xx_altivec_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif
EXTRA_DIST      += mpc6xx/altivec/README

# e500/clock
if e500_clock
noinst_PROGRAMS += e500/clock.rel
e500_clock_rel_SOURCES = mpc6xx/clock/c_clock.c mpc6xx/clock/c_clock.h
e500_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
e500_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
