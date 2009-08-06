##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/avr.h \
    rtems/score/cpu_asm.h rtems/score/types.h

include_rtems_avrdir = $(includedir)/avr
include_rtems_avr_HEADERS =  \
    avr/boot.h avr/common.h avr/crc16.h avr/delay.h avr/eeprom.h \
    avr/fuse.h avr/interrupt.h avr/io1200.h avr/io2313.h avr/io2323.h \
    avr/io2333.h avr/io2343.h avr/io43u32x.h avr/io43u35x.h avr/io4414.h \
    avr/io4433.h avr/io4434.h avr/io76c711.h avr/io8515.h avr/io8534.h \
    avr/io8535.h avr/io86r401.h avr/io90pwm1.h avr/io90pwm216.h \
    avr/io90pwm2b.h avr/io90pwm316.h avr/io90pwm3b.h avr/io90pwm81.h \
    avr/io90pwmx.h avr/io90scr100.h avr/ioa6289.h avr/ioat94k.h \
    avr/iocan128.h avr/iocan32.h avr/iocan64.h avr/iocanxx.h avr/io.h \
    avr/iom103.h avr/iom1280.h avr/iom1281.h avr/iom1284p.h avr/iom128.h \
    avr/iom128rfa1.h avr/iom161.h avr/iom162.h avr/iom163.h avr/iom164.h \
    avr/iom165.h avr/iom165p.h avr/iom168.h avr/iom168p.h avr/iom169.h \
    avr/iom169p.h avr/iom16.h avr/iom16hva.h avr/iom16m1.h avr/iom16u4.h \
    avr/iom2560.h avr/iom2561.h avr/iom323.h avr/iom324.h avr/iom3250.h \
    avr/iom325.h avr/iom328p.h avr/iom3290.h avr/iom329.h avr/iom32c1.h \
    avr/iom32.h avr/iom32hvb.h avr/iom32m1.h avr/iom32u4.h avr/iom32u6.h \
    avr/iom406.h avr/iom48.h avr/iom48p.h avr/iom640.h avr/iom644.h \
    avr/iom6450.h avr/iom645.h avr/iom6490.h avr/iom649.h avr/iom64c1.h \
    avr/iom64.h avr/iom64m1.h avr/iom8515.h avr/iom8535.h avr/iom88.h \
    avr/iom88p.h avr/iom8.h avr/iom8hva.h avr/iomx8.h avr/iomxx0_1.h \
    avr/iomxx4.h avr/iomxxhva.h avr/iotn11.h avr/iotn12.h avr/iotn13a.h \
    avr/iotn13.h avr/iotn15.h avr/iotn167.h avr/iotn22.h avr/iotn2313.h \
    avr/iotn24.h avr/iotn25.h avr/iotn261.h avr/iotn26.h avr/iotn28.h \
    avr/iotn43u.h avr/iotn44.h avr/iotn45.h avr/iotn461.h avr/iotn48.h \
    avr/iotn84.h avr/iotn85.h avr/iotn861.h avr/iotn87.h avr/iotn88.h \
    avr/iotnx4.h avr/iotnx5.h avr/iotnx61.h avr/iousb1286.h \
    avr/iousb1287.h avr/iousb162.h avr/iousb646.h avr/iousb647.h \
    avr/iousb82.h avr/iousbxx2.h avr/iousbxx6_7.h avr/iox128a1.h \
    avr/iox128a3.h avr/iox16a4.h avr/iox16d4.h avr/iox256a3b.h \
    avr/iox256a3.h avr/iox32a4.h avr/iox32d4.h avr/iox64a1.h avr/iox64a3.h \
    avr/lock.h avr/parity.h avr/pgmspace.h avr/portpins.h avr/power.h \
    avr/sfr_defs.h avr/signal.h avr/sleep.h avr/version.h avr/wdt.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
