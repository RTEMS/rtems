ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

###############################################################################
#                  Data                                                       #
###############################################################################

dist_project_lib_DATA = bsp_specs

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA =
project_lib_DATA += start.$(OBJEXT)
project_lib_DATA += startup/linkcmds
project_lib_DATA += startup/linkcmds.memory
project_lib_DATA += startup/linkcmds.intsram
project_lib_DATA += startup/linkcmds.sdram
project_lib_DATA += startup/linkcmds.qspiflash

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS =
include_HEADERS += ../../shared/include/tm27.h
include_HEADERS += ../shared/CMSIS/Include/core_cm7.h
include_HEADERS += ../shared/CMSIS/Include/core_cmInstr.h
include_HEADERS += ../shared/CMSIS/Include/core_cmFunc.h
include_HEADERS += ../shared/CMSIS/Include/core_cmSimd.h
include_HEADERS += ../shared/CMSIS/Include/cmsis_gcc.h
include_HEADERS += include/bsp.h

nodist_include_HEADERS =
nodist_include_HEADERS += ../../shared/include/coverhd.h
nodist_include_HEADERS += include/bspopts.h

include_bspdir = $(includedir)/bsp

nodist_include_bsp_HEADERS =
nodist_include_bsp_HEADERS += ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/armv7m/include/armv7m-irq.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/pin-config.h
include_bsp_HEADERS += include/atsam-i2c.h
include_bsp_HEADERS += include/i2c.h

include_libchipdir = $(includedir)/libchip

include_libchip_HEADERS =
include_libchip_HEADERS += libraries/libchip/chip.h
include_libchip_HEADERS += libraries/libchip/compiler.h

include_libchip_includedir = $(includedir)/libchip/include

include_libchip_include_HEADERS =
include_libchip_include_HEADERS += libraries/libchip/include/acc.h
include_libchip_include_HEADERS += libraries/libchip/include/adc.h
include_libchip_include_HEADERS += libraries/libchip/include/aes.h
include_libchip_include_HEADERS += libraries/libchip/include/afec.h
include_libchip_include_HEADERS += libraries/libchip/include/afe_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/chip.h
include_libchip_include_HEADERS += libraries/libchip/include/dac_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/efc.h
include_libchip_include_HEADERS += libraries/libchip/include/exceptions.h
include_libchip_include_HEADERS += libraries/libchip/include/flashd.h
include_libchip_include_HEADERS += libraries/libchip/include/gmacd.h
include_libchip_include_HEADERS += libraries/libchip/include/gmac.h
include_libchip_include_HEADERS += libraries/libchip/include/hsmci.h
include_libchip_include_HEADERS += libraries/libchip/include/icm.h
include_libchip_include_HEADERS += libraries/libchip/include/isi.h
include_libchip_include_HEADERS += libraries/libchip/include/iso7816_4.h
include_libchip_include_HEADERS += libraries/libchip/include/mcan.h
include_libchip_include_HEADERS += libraries/libchip/include/mcid.h
include_libchip_include_HEADERS += libraries/libchip/include/mediaLB.h
include_libchip_include_HEADERS += libraries/libchip/include/mpu.h
include_libchip_include_HEADERS += libraries/libchip/include/pio_capture.h
include_libchip_include_HEADERS += libraries/libchip/include/pio.h
include_libchip_include_HEADERS += libraries/libchip/include/pio_it.h
include_libchip_include_HEADERS += libraries/libchip/include/pmc.h
include_libchip_include_HEADERS += libraries/libchip/include/pwmc.h
include_libchip_include_HEADERS += libraries/libchip/include/qspi_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/qspi.h
include_libchip_include_HEADERS += libraries/libchip/include/rstc.h
include_libchip_include_HEADERS += libraries/libchip/include/rtc.h
include_libchip_include_HEADERS += libraries/libchip/include/rtt.h
include_libchip_include_HEADERS += libraries/libchip/include/sdramc.h
include_libchip_include_HEADERS += libraries/libchip/include/smc.h
include_libchip_include_HEADERS += libraries/libchip/include/spi_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/spi.h
include_libchip_include_HEADERS += libraries/libchip/include/ssc.h
include_libchip_include_HEADERS += libraries/libchip/include/supc.h
include_libchip_include_HEADERS += libraries/libchip/include/tc.h
include_libchip_include_HEADERS += libraries/libchip/include/timetick.h
include_libchip_include_HEADERS += libraries/libchip/include/trace.h
include_libchip_include_HEADERS += libraries/libchip/include/trng.h
include_libchip_include_HEADERS += libraries/libchip/include/twid.h
include_libchip_include_HEADERS += libraries/libchip/include/twi.h
include_libchip_include_HEADERS += libraries/libchip/include/uart_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/uart.h
include_libchip_include_HEADERS += libraries/libchip/include/usart_dma.h
include_libchip_include_HEADERS += libraries/libchip/include/usart.h
include_libchip_include_HEADERS += libraries/libchip/include/usbhs.h
include_libchip_include_HEADERS += libraries/libchip/include/video.h
include_libchip_include_HEADERS += libraries/libchip/include/wdt.h
include_libchip_include_HEADERS += libraries/libchip/include/xdmac.h
include_libchip_include_HEADERS += libraries/libchip/include/xdmad.h
include_libchip_include_HEADERS += libraries/libchip/include/xdma_hardware_interface.h

include_libchip_include_same70dir = $(includedir)/libchip/include/same70

include_libchip_include_same70_HEADERS =
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70j19.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70j20.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70j21.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70n19.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70n20.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70n21.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70q19.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70q20.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/same70q21.h
include_libchip_include_same70_HEADERS += libraries/libchip/include/same70/system_same70.h

include_libchip_include_same70_componentdir = $(includedir)/libchip/include/same70/component

include_libchip_include_same70_component_HEADERS =
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_acc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_aes.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_afec.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_chipid.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_dacc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_efc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_gmac.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_gpbr.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_hsmci.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_icm.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_isi.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_matrix.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_mcan.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_pio.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_pmc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_pwm.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_qspi.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_rstc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_rswdt.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_rtc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_rtt.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_sdramc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_smc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_spi.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_ssc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_supc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_tc.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_trng.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_twihs.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_uart.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_usart.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_usbhs.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_utmi.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_wdt.h
include_libchip_include_same70_component_HEADERS += libraries/libchip/include/same70/component/component_xdmac.h

include_libchip_include_same70_piodir = $(includedir)/libchip/include/same70/pio

include_libchip_include_same70_pio_HEADERS =
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70j19.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70j20.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70j21.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70n19.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70n20.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70n21.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70q19.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70q20.h
include_libchip_include_same70_pio_HEADERS += libraries/libchip/include/same70/pio/pio_same70q21.h

include_libchip_include_sams70dir = $(includedir)/libchip/include/sams70

include_libchip_include_sams70_HEADERS =
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70j19.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70j20.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70j21.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70n19.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70n20.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70n21.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70q19.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70q20.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/sams70q21.h
include_libchip_include_sams70_HEADERS += libraries/libchip/include/sams70/system_sams70.h

include_libchip_include_sams70_componentdir = $(includedir)/libchip/include/sams70/component

include_libchip_include_sams70_component_HEADERS =
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_acc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_aes.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_afec.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_chipid.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_dacc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_efc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_gpbr.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_hsmci.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_icm.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_isi.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_matrix.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_pio.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_pmc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_pwm.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_qspi.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_rstc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_rswdt.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_rtc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_rtt.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_sdramc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_smc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_spi.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_ssc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_supc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_tc.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_trng.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_twihs.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_uart.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_usart.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_usbhs.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_utmi.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_wdt.h
include_libchip_include_sams70_component_HEADERS += libraries/libchip/include/sams70/component/component_xdmac.h

include_libchip_include_sams70_piodir = $(includedir)/libchip/include/sams70/pio

include_libchip_include_sams70_pio_HEADERS =
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70j19.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70j20.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70j21.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70n19.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70n20.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70n21.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70q19.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70q20.h
include_libchip_include_sams70_pio_HEADERS += libraries/libchip/include/sams70/pio/pio_sams70q21.h

include_libchip_include_samv71dir = $(includedir)/libchip/include/samv71

include_libchip_include_samv71_HEADERS =
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71j19.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71j20.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71j21.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71n19.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71n20.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71n21.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71q19.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71q20.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/samv71q21.h
include_libchip_include_samv71_HEADERS += libraries/libchip/include/samv71/system_samv71.h

include_libchip_include_samv71_componentdir = $(includedir)/libchip/include/samv71/component

include_libchip_include_samv71_component_HEADERS =
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_acc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_aes.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_afec.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_chipid.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_dacc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_efc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_gmac.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_gpbr.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_hsmci.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_icm.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_isi.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_matrix.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_mcan.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_mlb.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_pio.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_pmc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_pwm.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_qspi.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_rstc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_rswdt.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_rtc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_rtt.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_sdramc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_smc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_spi.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_ssc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_supc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_tc.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_trng.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_twihs.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_uart.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_usart.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_usbhs.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_utmi.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_wdt.h
include_libchip_include_samv71_component_HEADERS += libraries/libchip/include/samv71/component/component_xdmac.h

include_libchip_include_samv71_piodir = $(includedir)/libchip/include/samv71/pio

include_libchip_include_samv71_pio_HEADERS =
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71j19.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71j20.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71j21.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71n19.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71n20.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71n21.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71q19.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71q20.h
include_libchip_include_samv71_pio_HEADERS += libraries/libchip/include/samv71/pio/pio_samv71q21.h

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c

# libchip
libbsp_a_SOURCES += libraries/libchip/source/acc.c
libbsp_a_SOURCES += libraries/libchip/source/aes.c
libbsp_a_SOURCES += libraries/libchip/source/afec.c
libbsp_a_SOURCES += libraries/libchip/source/afe_dma.c
libbsp_a_SOURCES += libraries/libchip/source/dac_dma.c
libbsp_a_SOURCES += libraries/libchip/source/efc.c
libbsp_a_SOURCES += libraries/libchip/source/exceptions.c
libbsp_a_SOURCES += libraries/libchip/source/flashd.c
libbsp_a_SOURCES += libraries/libchip/source/gmac.c
libbsp_a_SOURCES += libraries/libchip/source/gmacd.c
libbsp_a_SOURCES += libraries/libchip/source/hsmci.c
libbsp_a_SOURCES += libraries/libchip/source/icm.c
libbsp_a_SOURCES += libraries/libchip/source/isi.c
libbsp_a_SOURCES += libraries/libchip/source/mcan.c
libbsp_a_SOURCES += libraries/libchip/source/mediaLB.c
libbsp_a_SOURCES += libraries/libchip/source/mpu.c
libbsp_a_SOURCES += libraries/libchip/source/pio.c
libbsp_a_SOURCES += libraries/libchip/source/pio_capture.c
libbsp_a_SOURCES += libraries/libchip/source/pio_it.c
libbsp_a_SOURCES += libraries/libchip/source/pmc.c
libbsp_a_SOURCES += libraries/libchip/source/pwmc.c
libbsp_a_SOURCES += libraries/libchip/source/qspi.c
libbsp_a_SOURCES += libraries/libchip/source/qspi_dma.c
libbsp_a_SOURCES += libraries/libchip/source/rstc.c
libbsp_a_SOURCES += libraries/libchip/source/rtc.c
libbsp_a_SOURCES += libraries/libchip/source/rtt.c
libbsp_a_SOURCES += libraries/libchip/source/sdramc.c
libbsp_a_SOURCES += libraries/libchip/source/spi.c
libbsp_a_SOURCES += libraries/libchip/source/spi_dma.c
libbsp_a_SOURCES += libraries/libchip/source/ssc.c
libbsp_a_SOURCES += libraries/libchip/source/supc.c
libbsp_a_SOURCES += libraries/libchip/source/tc.c
libbsp_a_SOURCES += libraries/libchip/source/trng.c
libbsp_a_SOURCES += libraries/libchip/source/twi.c
libbsp_a_SOURCES += libraries/libchip/source/twid.c
libbsp_a_SOURCES += libraries/libchip/source/uart.c
libbsp_a_SOURCES += libraries/libchip/source/uart_dma.c
libbsp_a_SOURCES += libraries/libchip/source/usart.c
libbsp_a_SOURCES += libraries/libchip/source/usart_dma.c
libbsp_a_SOURCES += libraries/libchip/source/wdt.c
libbsp_a_SOURCES += libraries/libchip/source/xdmac.c
libbsp_a_SOURCES += libraries/libchip/source/xdmad.c
libbsp_a_SOURCES += libraries/libchip/source/xdma_hardware_interface.c

# libboard
libbsp_a_SOURCES += libraries/libboard/resources_v71/system_samv71.c
libbsp_a_SOURCES += libraries/libboard/source/board_lowlevel.c
libbsp_a_SOURCES += libraries/libboard/source/board_memories.c
libbsp_a_SOURCES += libraries/libboard/source/dbg_console.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/pin-config.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console.c
libbsp_a_SOURCES += console/debug-console.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c
libbsp_a_SOURCES += clock/systick-freq.c

# Timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/armv7m/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv7m/include

# Network
if HAS_NETWORKING
libbsp_a_SOURCES += network/if_atsam.c
endif

# I2C
libbsp_a_SOURCES += i2c/atsam_i2c_bus.c
libbsp_a_SOURCES += i2c/atsam_i2c_init.c

# RTC
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += rtc/rtc-config.c

# Includes
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/CMSIS/Include
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard/include
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip/include

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
