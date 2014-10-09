ACLOCAL_AMFLAGS = -I ../../../../aclocal

SUBDIRS = . tools

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
nodist_include_bsp_HEADERS += irq/irq.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

include_ndsdir = $(includedir)/nds
include_nds_HEADERS = touchscreen/touchscreen.h sound/sound.h

libbsp_a_CPPFLAGS = -DARM9
libbsp_a_CPPFLAGS += -I$(srcdir)/../../shared/include
libbsp_a_CPPFLAGS += -I$(srcdir)/include
libbsp_a_CPPFLAGS += -I$(srcdir)/libnds/include
libbsp_a_CPPFLAGS += -I$(srcdir)/libfat/source/disc_io
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += clock/clock.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += rtc/rtc.c
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += console/console.c
libbsp_a_SOURCES += fb/fb.c
libbsp_a_SOURCES += touchscreen/touchscreen.c
libbsp_a_SOURCES += touchscreen/parser.c
libbsp_a_SOURCES += touchscreen/reco.c
libbsp_a_SOURCES += timer/timer.c
libbsp_a_SOURCES += sound/sound.c
libbsp_a_SOURCES += block/block.c
libbsp_a_SOURCES += irq/irq.c
# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h

if HAS_NETWORKING
noinst_PROGRAMS += wifi.rel
wifi_rel_SOURCES = wifi/wifi.c wifi/compat.c
wifi_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM9 -I$(srcdir)/dswifi/include -I$(srcdir)/libnds/include -I$(srcdir)/dswifi/include -D_KERNEL -D__BSD_VISIBLE
wifi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# libnds, ARM9 side
noinst_PROGRAMS += libnds9.rel
bin2s:	$(srcdir)/tools/bin2s.c
	cc -o $(srcdir)/tools/bin2s $(srcdir)/tools/bin2s.c

SUFFIXES = .bin

%.s:	%.bin bin2s
	$(srcdir)/tools/bin2s $< > $(srcdir)/$@

libnds9_rel_SOURCES =  	libnds/source/common/biosCalls.S		\
			libnds/source/common/card.c			\
			libnds/source/common/gbfs.c			\
			libnds/source/common/interruptDispatcher.S	\
			libnds/source/common/interrupts.c		\
			libnds/source/arm9/boxtest.c			\
			libnds/source/arm9/default_font.S		\
			libnds/source/arm9/console.c			\
			libnds/source/arm9/COS.S			\
			libnds/source/arm9/dcache.S			\
			libnds/source/arm9/exceptionHandler.S		\
			libnds/source/arm9/exceptions.c			\
			libnds/source/arm9/gurumeditation.c		\
			libnds/source/arm9/icache.S			\
			libnds/source/arm9/image.c			\
			libnds/source/arm9/initSystem.c			\
			libnds/source/arm9/keys.c			\
			libnds/source/arm9/ndsmotion.c			\
			libnds/source/arm9/pcx.c			\
			libnds/source/arm9/rumble.c			\
			libnds/source/arm9/SIN.S			\
			libnds/source/arm9/sound.c			\
			libnds/source/arm9/system.c			\
			libnds/source/arm9/TAN.S			\
			libnds/source/arm9/touch.c			\
			libnds/source/arm9/video.c			\
			libnds/source/arm9/videoGL.c
libnds9_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM9 -I$(srcdir)/libnds/include -I$(srcdir)/include
libnds9_rel_CCASFLAGS = $(AM_CCASFLAGS) -DARM9 -I$(srcdir)/libnds/include
libnds9_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
# dswifi, ARM9 side
noinst_PROGRAMS += dswifi9.rel
dswifi9_rel_SOURCES = dswifi/arm9/source/wifi_arm9.c
dswifi9_rel_SOURCES += dswifi/common/source/spinlock.S
dswifi9_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM9 -I$(srcdir)/dswifi/include -I$(srcdir)/libnds/include -I$(srcdir)/dswifi/common/source -I$(srcdir)/wifi -D_KERNEL
dswifi9_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# dldi
noinst_PROGRAMS += libdldi.rel
libdldi_rel_SOURCES =  	libfat/source/disc_io/disc.c			\
			libfat/source/disc_io/io_cf_common.c		\
			libfat/source/disc_io/io_efa2.c			\
			libfat/source/disc_io/io_fcsr.c			\
			libfat/source/disc_io/io_m3cf.c			\
			libfat/source/disc_io/io_m3_common.c		\
			libfat/source/disc_io/io_m3sd.c			\
			libfat/source/disc_io/io_mpcf.c			\
			libfat/source/disc_io/io_njsd.c			\
			libfat/source/disc_io/io_nmmc.c			\
			libfat/source/disc_io/io_sccf.c			\
			libfat/source/disc_io/io_sc_common.c		\
			libfat/source/disc_io/io_scsd.c			\
			libfat/source/disc_io/io_sd_common.c		\
			libfat/source/disc_io/io_dldi.S			\
			libfat/source/disc_io/io_scsd_s.S
libdldi_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM9 -I$(srcdir)/libnds/include -I$(srcdir)/libfat/source/disc_io
libdldi_rel_CCASFLAGS = $(AM_CCASFLAGS) -DARM9 -I$(srcdir)/libnds/include -I$(srcdir)/libfat/source/disc_io
libdldi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# libnds, ARM7 side
noinst_PROGRAMS += libnds7.rel
libnds7_rel_SOURCES =   libnds/source/common/biosCalls.S		\
			libnds/source/common/card.c			\
			libnds/source/common/gbfs.c			\
			libnds/source/common/interruptDispatcher.S	\
			libnds/source/common/interrupts.c		\
			libnds/source/arm7/audio.c			\
			libnds/source/arm7/clock.c			\
			libnds/source/arm7/microphone.c			\
			libnds/source/arm7/spi.c			\
			libnds/source/arm7/touch.c			\
			libnds/source/arm7/userSettings.c
libnds7_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM7 -I$(srcdir)/libnds/include
libnds7_rel_CFLAGS = -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft
libnds7_rel_CCASFLAGS = -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft
libnds7_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
# dswifi, ARM7 side
noinst_PROGRAMS += dswifi7.rel
dswifi7_rel_SOURCES =  	dswifi/arm7/source/wifi_arm7.c			\
			dswifi/common/source/spinlock.S
dswifi7_rel_CPPFLAGS = $(AM_CPPFLAGS) -DARM7 -I$(srcdir)/dswifi/include -I$(srcdir)/libnds/include -I$(srcdir)/dswifi/common/source
dswifi7_rel_CFLAGS = -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft
dswifi7_rel_CCASFLAGS = -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft
dswifi7_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# include libnds to dist so the user program can include it
include_libndsdir = $(includedir)/libnds
include_libnds_HEADERS =	libnds/include/gbfs.h			\
				libnds/include/nds.h

include_libndsndsdir = $(includedir)/libnds/nds
include_libndsnds_HEADERS =	libnds/include/nds/memory.h		\
				libnds/include/nds/system.h		\
				libnds/include/nds/bios.h		\
				libnds/include/nds/registers_alt.h	\
				libnds/include/nds/interrupts.h		\
				libnds/include/nds/card.h		\
				libnds/include/nds/ipc.h		\
				libnds/include/nds/timers.h		\
				libnds/include/nds/dma.h		\
				libnds/include/nds/reload.h		\
				libnds/include/nds/jtypes.h

include_libndsnds9dir = $(includedir)/libnds/nds/arm9
include_libndsnds9_HEADERS =	libnds/include/nds/arm9/ndsmotion.h	\
				libnds/include/nds/arm9/pcx.h		\
				libnds/include/nds/arm9/input.h		\
				libnds/include/nds/arm9/math.h		\
				libnds/include/nds/arm9/console.h	\
				libnds/include/nds/arm9/sprite.h	\
				libnds/include/nds/arm9/videoGL.h	\
				libnds/include/nds/arm9/cache.h		\
				libnds/include/nds/arm9/image.h		\
				libnds/include/nds/arm9/trig_lut.h	\
				libnds/include/nds/arm9/video.h		\
				libnds/include/nds/arm9/exceptions.h	\
				libnds/include/nds/arm9/rumble.h	\
				libnds/include/nds/arm9/background.h	\
				libnds/include/nds/arm9/boxtest.h	\
				libnds/include/nds/arm9/postest.h	\
				libnds/include/nds/arm9/sound.h

include_libndsnds7dir = $(includedir)/libnds/nds/arm7
include_libndsnds7_HEADERS =	libnds/include/nds/arm7/serial.h	\
				libnds/include/nds/arm7/audio.h		\
				libnds/include/nds/arm7/clock.h		\
				libnds/include/nds/arm7/touch.h
EXTRA_DIST = coproc/coproc.S coproc/coproc.c
if HAS_NETWORKING
coproc.bin: coproc/coproc.S coproc/coproc.c libnds7.rel dswifi7.rel
	@CC@ -o coproc.elf -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft -O2 -DARM7 -I$(srcdir)/libnds/include -I$(srcdir)/dswifi/include -DENABLE_WIFI -T $(srcdir)/coproc/coproc.ld $^ -lc
	$(OBJCOPY) -O binary coproc.elf coproc.bin
else
coproc.bin: coproc/coproc.S coproc/coproc.c libnds7.rel
	@CC@ -o coproc.elf -mcpu=arm7tdmi -mfpu=vfp -mfloat-abi=soft -O2 -DARM7 -I$(srcdir)/libnds/include -T $(srcdir)/coproc/coproc.ld $^ -lc
	$(OBJCOPY) -O binary coproc.elf coproc.bin
endif
project_lib_DATA += coproc.bin

libbsp_a_LIBADD = libnds9.rel libdldi.rel

if HAS_NETWORKING
    libbsp_a_LIBADD += wifi.rel dswifi9.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
