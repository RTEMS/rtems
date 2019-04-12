/**
 * @file
 * @ingroup amba
 * @brief AMBA Plug & Play Bus Vendor and Device IDs
 */

/*
 *  COPYRIGHT (c) 2008.
 *  Gaisler Research
 *
 *  This header file provide all known VENDOR and DEVICE IDs available 
 *  in the AMBA Plug & Play information. Taken from GRLIB 3386.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __AMBAPP_DEVS_H__
#define __AMBAPP_DEVS_H__

/* Vendor codes */
#define VENDOR_RESERVED      0x00
#define VENDOR_GAISLER       0x01
#define VENDOR_PENDER        0x02
#define VENDOR_ESA           0x04
#define VENDOR_ASTRIUM       0x06
#define VENDOR_OPENCHIP      0x07
#define VENDOR_OPENCORES     0x08
#define VENDOR_CONTRIB       0x09
#define VENDOR_DLR           0x0a
#define VENDOR_EONIC         0x0b
#define VENDOR_TELECOMPT     0x0c
#define VENDOR_DTU           0x0d
#define VENDOR_BSC           0x0e
#define VENDOR_RADIONOR      0x0f
#define VENDOR_GLEICHMANN    0x10
#define VENDOR_MENTA         0x11
#define VENDOR_SUN           0x13
#define VENDOR_MOVIDIA       0x14
#define VENDOR_ORBITA        0x17
#define VENDOR_SIEMENS       0x1a
#define VENDOR_SYNOPSYS      0x21
#define VENDOR_NASA          0x22
#define VENDOR_NIIET         0x23
#define VENDOR_S3            0x31
#define VENDOR_ACTEL         0xac
#define VENDOR_APPLECORE     0xae
#define VENDOR_C3E           0xc3
#define VENDOR_CBKPAN        0xc8
#define VENDOR_CAL           0xca
#define VENDOR_CETON         0xcb
#define VENDOR_EMBEDDIT      0xea
#define VENDOR_NASA_GSFC     0xfc
#define VENDOR_AZST          0xfe

/* Gaisler Research device id's */
#define GAISLER_LEON2DSU     0x002
#define GAISLER_LEON3        0x003
#define GAISLER_LEON3DSU     0x004
#define GAISLER_ETHAHB       0x005
#define GAISLER_APBMST       0x006
#define GAISLER_AHBUART      0x007
#define GAISLER_SRCTRL       0x008
#define GAISLER_SDCTRL       0x009
#define GAISLER_SSRCTRL      0x00a
#define GAISLER_I2C2AHB      0x00b
#define GAISLER_APBUART      0x00c
#define GAISLER_IRQMP        0x00d
#define GAISLER_AHBRAM       0x00e
#define GAISLER_AHBDPRAM     0x00f
#define GAISLER_GRIOMMU2     0x010
#define GAISLER_GPTIMER      0x011
#define GAISLER_PCITRG       0x012
#define GAISLER_PCISBRG      0x013
#define GAISLER_PCIFBRG      0x014
#define GAISLER_PCITRACE     0x015
#define GAISLER_DMACTRL      0x016
#define GAISLER_AHBTRACE     0x017
#define GAISLER_DSUCTRL      0x018
#define GAISLER_CANAHB       0x019
#define GAISLER_GPIO         0x01a
#define GAISLER_AHBROM       0x01b
#define GAISLER_AHBJTAG      0x01c
#define GAISLER_ETHMAC       0x01d
#define GAISLER_SWNODE       0x01e
#define GAISLER_SPW          0x01f
#define GAISLER_AHB2AHB      0x020
#define GAISLER_USBDC        0x021
#define GAISLER_USB_DCL      0x022
#define GAISLER_DDRMP        0x023
#define GAISLER_ATACTRL      0x024
#define GAISLER_DDRSP        0x025
#define GAISLER_EHCI         0x026
#define GAISLER_UHCI         0x027
#define GAISLER_I2CMST       0x028
#define GAISLER_SPW2         0x029
#define GAISLER_AHBDMA       0x02a
#define GAISLER_NUHOSP3      0x02b
#define GAISLER_CLKGATE      0x02c
#define GAISLER_SPICTRL      0x02d
#define GAISLER_DDR2SP       0x02e
#define GAISLER_SLINK        0x02f
#define GAISLER_GRTM         0x030
#define GAISLER_GRTC         0x031
#define GAISLER_GRPW         0x032
#define GAISLER_GRCTM        0x033
#define GAISLER_GRHCAN       0x034
#define GAISLER_GRFIFO       0x035
#define GAISLER_GRADCDAC     0x036
#define GAISLER_GRPULSE      0x037
#define GAISLER_GRTIMER      0x038
#define GAISLER_AHB2PP       0x039
#define GAISLER_GRVERSION    0x03a
#define GAISLER_APB2PW       0x03b
#define GAISLER_PW2APB       0x03c
#define GAISLER_GRCAN        0x03d
#define GAISLER_I2CSLV       0x03e
#define GAISLER_U16550       0x03f
#define GAISLER_AHBMST_EM    0x040
#define GAISLER_AHBSLV_EM    0x041
#define GAISLER_GRTESTMOD    0x042
#define GAISLER_ASCS         0x043
#define GAISLER_IPMVBCTRL    0x044
#define GAISLER_SPIMCTRL     0x045
#define GAISLER_L4STAT       0x047
#define GAISLER_LEON4        0x048
#define GAISLER_LEON4DSU     0x049
#define GAISLER_GRPWM        0x04a
#define GAISLER_PWM          0x04a
#define GAISLER_L2CACHE      0x04b
#define GAISLER_SDCTRL64     0x04c
#define GAISLER_GR1553B      0x04d
#define GAISLER_1553TST      0x04e
#define GAISLER_GRIOMMU      0x04f
#define GAISLER_FTAHBRAM     0x050
#define GAISLER_FTSRCTRL     0x051
#define GAISLER_AHBSTAT      0x052
#define GAISLER_LEON3FT      0x053
#define GAISLER_FTMCTRL      0x054
#define GAISLER_FTSDCTRL     0x055
#define GAISLER_FTSRCTRL8    0x056
#define GAISLER_MEMSCRUB     0x057
#define GAISLER_FTSDCTRL64   0x058
#define GAISLER_NANDFCTRL    0x059
#define GAISLER_N2DLLCTRL    0x05a
#define GAISLER_N2PLLCTRL    0x05b
#define GAISLER_SPI2AHB      0x05c
#define GAISLER_DDRSDMUX     0x05d
#define GAISLER_AHBFROM      0x05e
#define GAISLER_PCIEXP       0x05f
#define GAISLER_APBPS2       0x060
#define GAISLER_VGACTRL      0x061
#define GAISLER_LOGAN        0x062
#define GAISLER_SVGACTRL     0x063
#define GAISLER_T1AHB        0x064
#define GAISLER_MP7WRAP      0x065
#define GAISLER_GRSYSMON     0x066
#define GAISLER_GRACECTRL    0x067
#define GAISLER_ATAHBSLV     0x068
#define GAISLER_ATAHBMST     0x069
#define GAISLER_ATAPBSLV     0x06a
#define GAISLER_MIGDDR2      0x06b
#define GAISLER_LCDCTRL      0x06c
#define GAISLER_SWITCHOVER   0x06d
#define GAISLER_FIFOUART     0x06e
#define GAISLER_MUXCTRL      0x06f
#define GAISLER_B1553BC      0x070
#define GAISLER_B1553RT      0x071
#define GAISLER_B1553BRM     0x072
#define GAISLER_GRAES        0x073
#define GAISLER_AES          0x073
#define GAISLER_ECC          0x074
#define GAISLER_PCIF         0x075
#define GAISLER_CLKMOD       0x076
#define GAISLER_HAPSTRAK     0x077
#define GAISLER_TEST_1X2     0x078
#define GAISLER_WILD2AHB     0x079
#define GAISLER_BIO1         0x07a
#define GAISLER_GRAESDMA     0x07b
#define GAISLER_AESDMA       0x07b
#define GAISLER_GRPCI2       0x07c
#define GAISLER_GRPCI2_DMA   0x07d
#define GAISLER_GRPCI2_TB    0x07e
#define GAISLER_MMA          0x07f
#define GAISLER_SATCAN       0x080
#define GAISLER_CANMUX       0x081
#define GAISLER_GRTMRX       0x082
#define GAISLER_GRTCTX       0x083
#define GAISLER_GRTMDESC     0x084
#define GAISLER_GRTMVC       0x085
#define GAISLER_GEFFE        0x086
#define GAISLER_GPREG        0x087
#define GAISLER_GRTMPAHB     0x088
#define GAISLER_SPWCUC       0x089
#define GAISLER_SPW2_DMA     0x08a
#define GAISLER_SPW_ROUTER   0x08b
#define GAISLER_SPWROUTER    0x08b
#define GAISLER_EDCLMST      0x08c
#define GAISLER_GRPWTX       0x08d
#define GAISLER_GRPWRX       0x08e
#define GAISLER_GPREGBANK    0x08f
#define GAISLER_MIG_7SERIES  0x090
#define GAISLER_GRSPW2_SIST  0x091
#define GAISLER_SGMII        0x092
#define GAISLER_RGMII        0x093
#define GAISLER_IRQGEN       0x094
#define GAISLER_GRDMAC       0x095
#define GAISLER_AHB2AVLA     0x096
#define GAISLER_SPWTDP       0x097
#define GAISLER_L3STAT       0x098
#define GAISLER_GR740THS     0x099
#define GAISLER_GRRM         0x09a
#define GAISLER_CMAP         0x09b
#define GAISLER_CPGEN        0x09c
#define GAISLER_AMBAPROT     0x09d
#define GAISLER_IGLOO2_BRIDGE 0x09e
#define GAISLER_AHB2AXI      0x09f
#define GAISLER_AXI2AHB      0x0a0
#define GAISLER_FDIR_RSTCTRL 0x0a1
#define GAISLER_APB3MST      0x0a2
#define GAISLER_LRAM         0x0a3
#define GAISLER_BOOTSEQ      0x0a4
#define GAISLER_TCCOP        0x0a5
#define GAISLER_SPIMASTER    0x0a6
#define GAISLER_SPISLAVE     0x0a7
#define GAISLER_GRSRIO       0x0a8
#define GAISLER_AHBLM2AHB    0x0a9
#define GAISLER_AHBS2NOC     0x0aa
#define GAISLER_TCAU         0x0ab
#define GAISLER_GRTMDYNVCID  0x0ac
#define GAISLER_RNOCIRQPROP  0x0ad
#define GAISLER_FTADDR       0x0ae
#define GAISLER_ATG          0x0b0
#define GAISLER_DFITRACE     0x0b1
#define GAISLER_SELFTEST     0x0b2
#define GAISLER_DFIERRINJ    0x0b3
#define GAISLER_DFICHECK     0x0b4
#define GAISLER_GRCANFD      0x0b5
#define GAISLER_NIM          0x0b6
#define GAISLER_BANDGAP      0x1f0
#define GAISLER_MPROT        0x1f1
#define GAISLER_ADC          0x1f2
#define GAISLER_BO           0x1f3
#define GAISLER_DAC          0x1f4
#define GAISLER_PLL          0x1f6

#define GAISLER_PIPEWRAPPER  0xffa
#define GAISLER_L2TIME       0xffd  /* internal device: leon2 timer */
#define GAISLER_L2C          0xffe  /* internal device: leon2compat */
#define GAISLER_PLUGPLAY     0xfff  /* internal device: plug & play configarea */

/* European Space Agency device id's */
#define ESA_LEON2            0x002
#define ESA_LEON2APB         0x003
#define ESA_IRQ              0x005
#define ESA_TIMER            0x006
#define ESA_UART             0x007
#define ESA_CFG              0x008
#define ESA_IO               0x009
#define ESA_MCTRL            0x00f
#define ESA_PCIARB           0x010
#define ESA_HURRICANE        0x011
#define ESA_SPW_RMAP         0x012
#define ESA_SPW2             0x012
#define ESA_AHBUART          0x013
#define ESA_SPWA             0x014
#define ESA_BOSCHCAN         0x015
#define ESA_IRQ2             0x016
#define ESA_AHBSTAT          0x017
#define ESA_WPROT            0x018
#define ESA_WPROT2           0x019
#define ESA_PDEC3AMBA        0x020
#define ESA_PTME3AMBA        0x021

#define OPENCHIP_APBGPIO     0x001
#define OPENCHIP_APBI2C      0x002
#define OPENCHIP_APBSPI      0x003
#define OPENCHIP_APBCHARLCD  0x004
#define OPENCHIP_APBPWM      0x005
#define OPENCHIP_APBPS2      0x006
#define OPENCHIP_APBMMCSD    0x007
#define OPENCHIP_APBNAND     0x008
#define OPENCHIP_APBLPC      0x009
#define OPENCHIP_APBCF       0x00a
#define OPENCHIP_APBSYSACE   0x00b
#define OPENCHIP_APB1WIRE    0x00c
#define OPENCHIP_APBJTAG     0x00d
#define OPENCHIP_APBSUI      0x00e


#define CONTRIB_CORE1        0x001
#define CONTRIB_CORE2        0x002

#define GLEICHMANN_CUSTOM    0x001
#define GLEICHMANN_GEOLCD01  0x002
#define GLEICHMANN_DAC       0x003
#define GLEICHMANN_HPI       0x004
#define GLEICHMANN_SPI       0x005
#define GLEICHMANN_HIFC      0x006
#define GLEICHMANN_ADCDAC    0x007
#define GLEICHMANN_SPIOC     0x008
#define GLEICHMANN_AC97      0x009

#define SUN_T1               0x001
#define SUN_S1               0x011

#define ORBITA_1553B         0x001
#define ORBITA_429           0x002
#define ORBITA_SPI           0x003
#define ORBITA_I2C           0x004
#define ORBITA_SMARTCARD     0x064
#define ORBITA_SDCARD        0x065
#define ORBITA_UART16550     0x066
#define ORBITA_CRYPTO        0x067
#define ORBITA_SYSIF         0x068
#define ORBITA_PIO           0x069
#define ORBITA_RTC           0x0c8
#define ORBITA_COLORLCD      0x12c
#define ORBITA_PCI           0x190
#define ORBITA_DSP           0x1f4
#define ORBITA_USBHOST       0x258
#define ORBITA_USBDEV        0x2bc

#define NASA_EP32            0x001

#define CAL_DDRCTRL          0x188

#define ACTEL_COREMP7        0x001

/* Opencores device id's */
#define OPENCORES_PCIBR  0x4
#define OPENCORES_ETHMAC 0x5

#endif
