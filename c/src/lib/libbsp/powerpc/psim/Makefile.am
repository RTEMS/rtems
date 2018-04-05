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

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../shared/bspgetworkarea.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c

# pclock
librtemsbsp_a_SOURCES += ../shared/clock/p_clock.c
# console
librtemsbsp_a_SOURCES += ../../shared/console-polled.c
# debugio
librtemsbsp_a_SOURCES += console/console-io.c console/consupp.S

# irq
librtemsbsp_a_SOURCES += irq/irq_init.c ../shared/irq/openpic_i8259_irq.c \
    ../shared/openpic/openpic.c \
    ../../../../../../bsps/shared/irq/irq-default-handler.c

# vectors
librtemsbsp_a_SOURCES += vectors/align_h.S

if HAS_MP
# shmdr
librtemsbsp_a_SOURCES += \
    shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c shmsupp/mpisr.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/if_sim.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/psim/headers.am
