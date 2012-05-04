ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h 
include_HEADERS += include/tm27.h
include_HEADERS += ../shared/include/asm.h 
include_HEADERS += ../shared/include/traptable.h 

## these are the include files used by the boot process from HelenOS Sparc64
include_bootdir = $(includedir)/boot
include_boot_HEADERS = \
  ../shared/helenos/boot/include/align.h \
  ../shared/helenos/boot/include/balloc.h \
  ../shared/helenos/boot/include/gentypes.h \
  ../shared/helenos/boot/include/main.h \
  ../shared/helenos/boot/include/ofwarch.h \
  ../shared/helenos/boot/include/ofw.h \
  ../shared/helenos/boot/include/ofw_tree.h \
  ../shared/helenos/boot/include/register.h \
  ../shared/helenos/boot/include/stack.h \
  ../shared/helenos/boot/include/types.h 

## These include files mimic the HelenOS kernel include layout, which 
## in HelenOS-0.4.2 is more complicated than the boot include dirs.

# assumed by the HelenOS sources to be in the root includedir. We place them
# in a kernel include directory.
include_kerneldir = $(includedir)/kernel
include_kernel_HEADERS = \
  ../shared/helenos/kernel/generic/include/align.h

include_archdir = $(includedir)/arch
include_arch_HEADERS = \
  ../shared/helenos/kernel/sparc64/include/arch.h \
  ../shared/helenos/kernel/sparc64/include/boot.h \
  ../shared/helenos/kernel/sparc64/include/regdef.h \
  ../shared/helenos/kernel/sparc64/include/stack.h 

include_arch_sun4udir = $(includedir)/arch/sun4u
include_arch_sun4u_HEADERS = \
  ../shared/helenos/kernel/sparc64/include/sun4u/arch.h 

include_arch_mmdir = $(includedir)/arch/mm
include_arch_mm_HEADERS = \
  ../shared/helenos/kernel/sparc64/include/mm/cache_spec.h \
  ../shared/helenos/kernel/sparc64/include/mm/frame.h \
  ../shared/helenos/kernel/sparc64/include/mm/mmu.h \
  ../shared/helenos/kernel/sparc64/include/mm/page.h \
  ../shared/helenos/kernel/sparc64/include/mm/tlb.h \
  ../shared/helenos/kernel/sparc64/include/mm/tte.h


include_arch_mm_sun4udir = $(includedir)/arch/mm/sun4u
include_arch_mm_sun4u_HEADERS = \
  ../shared/helenos/kernel/sparc64/include/mm/sun4u/frame.h \
  ../shared/helenos/kernel/sparc64/include/mm/sun4u/mmu.h \
  ../shared/helenos/kernel/sparc64/include/mm/sun4u/page.h \
  ../shared/helenos/kernel/sparc64/include/mm/sun4u/tlb.h \
  ../shared/helenos/kernel/sparc64/include/mm/sun4u/tte.h

include_genarch_ofwdir = $(includedir)/genarch/ofw
include_genarch_ofw_HEADERS = \
  ../shared/helenos/kernel/genarch/include/ofw/ofw_tree.h


nodist_include_HEADERS = include/bspopts.h    
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h 
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h


EXTRA_DIST = ../shared/start/start.S 
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bspstart.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppost.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../shared/startup/bspgetworkarea.c \
    ../shared/startup/bsppredriverhook.c \
    ../shared/startup/setvec.c \
    ../../shared/gnatinstallhandler.c  \
    ../shared/helenos/boot/genarch/balloc.c \
    ../shared/helenos/boot/genarch/ofw.c \
    ../shared/helenos/boot/genarch/ofw_tree.c \
    ../shared/helenos/boot/generic/string.c \
    ../shared/helenos/boot/sparc64/loader/ofwarch.c \
    ../shared/helenos/boot/sparc64/loader/main.c \
    ../shared/helenos/boot/sparc64/loader/ofwasm.S \
    ../shared/asm/asm.S \
    start/bspinit.S

mmu_SOURCES = \
    ../shared/helenos/kernel/sparc64/src/cache.S \
    ../shared/helenos/kernel/sparc64/src/sun4u/takemmu.S \
    ../shared/start/trap_table.S

#clock_SOURCES = ../../shared/clock_driver_simidle.c
clock_SOURCES = ../shared/clock/ckinit.c

console_SOURCES = ../../shared/console.c  ../shared/console/conscfg.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c

timer_SOURCES = ../../shared/timerstub.c

noinst_LIBRARIES = libbsp.a 
libbsp_a_SOURCES = $(startup_SOURCES) $(mmu_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)


libbsp_a_LIBADD = \
      ../../../libcpu/@RTEMS_CPU@/shared/shared-score.rel \
      ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
      ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel \
      ../../../libcpu/@RTEMS_CPU@/shared/sparc64-syscall.rel 

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
