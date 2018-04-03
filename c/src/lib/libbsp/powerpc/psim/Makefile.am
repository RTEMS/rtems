ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../shared/bspgetworkarea.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c

# pclock
libbsp_a_SOURCES += ../shared/clock/p_clock.c
# console
libbsp_a_SOURCES += ../../shared/console-polled.c
# debugio
libbsp_a_SOURCES += console/console-io.c console/consupp.S

# irq
libbsp_a_SOURCES += irq/irq_init.c ../shared/irq/openpic_i8259_irq.c \
    ../shared/openpic/openpic.c ../../shared/src/irq-server.c  \
    ../../shared/src/irq-generic.c ../../shared/src/irq-default-handler.c

# vectors
libbsp_a_SOURCES += vectors/align_h.S

if HAS_MP
# shmdr
libbsp_a_SOURCES += \
    shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c shmsupp/mpisr.c
endif

if HAS_NETWORKING
libbsp_a_SOURCES += network/if_sim.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/psim/headers.am
