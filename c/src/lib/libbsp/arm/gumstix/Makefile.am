ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_SOURCES += \
  ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
  ../../shared/bootcard.c ../../shared/sbrk.c \
  ../../shared/getentropy-cpucounter.c \
  ../../shared/gnatinstallhandler.c \
  ../../shared/bspclean.c startup/bspstart.c startup/bspreset.c startup/memmap.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += clock/clock.c
librtemsbsp_a_SOURCES += timer/timer.c

#console
librtemsbsp_a_SOURCES += console/uarts.c ../../shared/console.c \
  ../../shared/console_select.c  ../../shared/console_control.c \
  ../../shared/console_read.c ../../shared/console_write.c 
librtemsbsp_a_SOURCES += console/ffuart.c
# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if ON_SKYEYE
librtemsbsp_a_SOURCES += fb/fb.c
if HAS_NETWORKING
librtemsbsp_a_SOURCES += rtl8019/rtl8019.c rtl8019/wd80x3.h
endif
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm920-mmu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/gumstix/headers.am
