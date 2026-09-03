/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  AMBA Plug & Play Device and Vendor name database: Created from GRLIB 4300.
 *
 *  COPYRIGHT (c) 2026.
 *  Frontgrade Gaisler.
 *
 *  The device and vendor definitions are extracted with a script from
 *  GRLIB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <grlib/ambapp.h>
#include <grlib/ambapp_ids.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

typedef struct {
  int   device_id;
  char *name;
} ambapp_device_name;

typedef struct {
  unsigned int        vendor_id;
  char               *name;
  ambapp_device_name *devices;
} ambapp_vendor_devnames;

/**************** AUTO GENERATED FROM devices.vhd ****************/
static ambapp_device_name GAISLER_devices[] = {
  { GAISLER_LEON2DSU, "LEON2 Debug Support Unit" },
  { GAISLER_LEON3, "LEON3 SPARC V8 Processor" },
  { GAISLER_LEON3DSU, "LEON3 Debug Support Unit" },
  { GAISLER_ETHAHB, "OC ethernet AHB interface" },
  { GAISLER_APBMST, "AHB/APB Bridge" },
  { GAISLER_AHBUART, "AHB Debug UART" },
  { GAISLER_SRCTRL, "Simple SRAM Controller" },
  { GAISLER_SDCTRL, "PC133 SDRAM Controller" },
  { GAISLER_SSRCTRL, "Synchronous SRAM Controller" },
  { GAISLER_I2C2AHB, "I2C to AHB Bridge" },
  { GAISLER_APBUART, "Generic UART" },
  { GAISLER_IRQMP, "Multi-processor Interrupt Ctrl." },
  { GAISLER_AHBRAM, "Single-port AHB SRAM module" },
  { GAISLER_AHBDPRAM, "Dual-port AHB SRAM module" },
  { GAISLER_GRIOMMU2, "IOMMU secondary master i/f" },
  { GAISLER_GPTIMER, "Modular Timer Unit" },
  { GAISLER_PCITRG, "Simple 32-bit PCI Target" },
  { GAISLER_PCISBRG, "Simple 32-bit PCI Bridge" },
  { GAISLER_PCIFBRG, "Fast 32-bit PCI Bridge" },
  { GAISLER_PCITRACE, "32-bit PCI Trace Buffer" },
  { GAISLER_DMACTRL, "PCI/AHB DMA controller" },
  { GAISLER_AHBTRACE, "AMBA Trace Buffer" },
  { GAISLER_DSUCTRL, "DSU/ETH controller" },
  { GAISLER_CANAHB, "OC CAN AHB interface" },
  { GAISLER_GPIO, "General Purpose I/O port" },
  { GAISLER_AHBROM, "Generic AHB ROM" },
  { GAISLER_AHBJTAG, "JTAG Debug Link" },
  { GAISLER_ETHMAC, "GR Ethernet MAC" },
  { GAISLER_SWNODE, "SpaceWire Node Interface" },
  { GAISLER_SPW, "SpaceWire Serial Link" },
  { GAISLER_AHB2AHB, "AHB-to-AHB Bridge" },
  { GAISLER_USBDC, "GR USB 2.0 Device Controller" },
  { GAISLER_USB_DCL, "USB Debug Communication Link" },
  { GAISLER_DDRMP, "Multi-port DDR controller" },
  { GAISLER_ATACTRL, "ATA controller" },
  { GAISLER_DDRSP, "Single-port DDR266 controller" },
  { GAISLER_EHCI, "USB Enhanced Host Controller" },
  { GAISLER_UHCI, "USB Universal Host Controller" },
  { GAISLER_I2CMST, "AMBA Wrapper for OC I2C-master" },
  { GAISLER_SPW2, "GRSPW2 SpaceWire Serial Link" },
  { GAISLER_AHBDMA, "Simple AHB DMA controller" },
  { GAISLER_NUHOSP3, "Nuhorizons Spartan3 IO I/F" },
  { GAISLER_CLKGATE, "Clock gating unit" },
  { GAISLER_SPICTRL, "SPI Controller" },
  { GAISLER_DDR2SP, "Single-port DDR2 controller" },
  { GAISLER_SLINK, "SLINK Master" },
  { GAISLER_GRTM, "CCSDS Telemetry Encoder" },
  { GAISLER_GRTC, "CCSDS Telecommand Decoder" },
  { GAISLER_GRPW, "PacketWire to AMBA AHB I/F" },
  { GAISLER_GRCTM, "CCSDS Time Manager" },
  { GAISLER_GRHCAN, "ESA HurriCANe CAN with DMA" },
  { GAISLER_GRFIFO, "FIFO Controller" },
  { GAISLER_GRADCDAC, "ADC / DAC Interface" },
  { GAISLER_GRPULSE, "General Purpose I/O with Pulses" },
  { GAISLER_GRTIMER, "Timer Unit with Latches" },
  { GAISLER_AHB2PP, "AMBA AHB to Packet Parallel I/F" },
  { GAISLER_GRVERSION, "Version and Revision Register" },
  { GAISLER_APB2PW, "PacketWire Transmit Interface" },
  { GAISLER_PW2APB, "PacketWire Receive Interface" },
  { GAISLER_GRCAN, "CAN Controller with DMA" },
  { GAISLER_I2CSLV, "I2C Slave" },
  { GAISLER_U16550, "Simple 16550 UART" },
  { GAISLER_AHBMST_EM, "AMBA Master Emulator" },
  { GAISLER_AHBSLV_EM, "AMBA Slave Emulator" },
  { GAISLER_GRTESTMOD, "Test report module" },
  { GAISLER_ASCS, "ASCS Master" },
  { GAISLER_IPMVBCTRL, "IPM-bus/MVBC memory controller" },
  { GAISLER_SPIMCTRL, "SPI Memory Controller" },
  { GAISLER_L4STAT, "LEON4 Statistics Unit" },
  { GAISLER_LEON4, "LEON4 SPARC V8 Processor" },
  { GAISLER_LEON4DSU, "LEON4 Debug Support Unit" },
  { GAISLER_PWM, "PWM generator" },
  { GAISLER_L2CACHE, "L2-Cache Controller" },
  { GAISLER_SDCTRL64, "64-bit FT SDRAM Controller" },
  { GAISLER_GR1553B, "MIL-STD-1553B Interface" },
  { GAISLER_1553TST, "MIL-STD-1553B Test Device" },
  { GAISLER_GRIOMMU, "IO Memory Management Unit" },
  { GAISLER_FTAHBRAM, "Generic FT AHB SRAM module" },
  { GAISLER_FTSRCTRL, "Simple FT SRAM Controller" },
  { GAISLER_AHBSTAT, "AHB Status Register" },
  { GAISLER_LEON3FT, "LEON3FT SPARC V8 Processor" },
  { GAISLER_FTMCTRL, "Memory controller with EDAC" },
  { GAISLER_FTSDCTRL, "FT PC133 SDRAM Controller" },
  { GAISLER_FTSRCTRL8, "FT 8-bit SRAM/16-bit IO Ctrl" },
  { GAISLER_MEMSCRUB, "AHB Memory Scrubber" },
  { GAISLER_FTSDCTRL64, "64-bit FT SDRAM Controller" },
  { GAISLER_NANDFCTRL, "NAND Flash Controller" },
  { GAISLER_N2DLLCTRL, "N2X DLL Dynamic Config. i/f" },
  { GAISLER_N2PLLCTRL, "N2X PLL Dynamic Config. i/f" },
  { GAISLER_SPI2AHB, "SPI to AHB Bridge" },
  { GAISLER_DDRSDMUX, "Muxed FT DDR/SDRAM controller" },
  { GAISLER_AHBFROM, "Flash ROM Memory" },
  { GAISLER_PCIEXP, "Xilinx PCI EXPRESS Wrapper" },
  { GAISLER_APBPS2, "PS2 interface" },
  { GAISLER_VGACTRL, "VGA controller" },
  { GAISLER_LOGAN, "On chip Logic Analyzer" },
  { GAISLER_SVGACTRL, "SVGA frame buffer" },
  { GAISLER_T1AHB, "Niagara T1 PCX/AHB bridge" },
  { GAISLER_MP7WRAP, "CoreMP7 wrapper" },
  { GAISLER_GRSYSMON, "AMBA wrapper for System Monitor" },
  { GAISLER_GRACECTRL, "System ACE I/F Controller" },
  { GAISLER_ATAHBSLV, "AMBA Test Framework AHB Slave" },
  { GAISLER_ATAHBMST, "AMBA Test Framework AHB Master" },
  { GAISLER_ATAPBSLV, "AMBA Test Framework APB Slave" },
  { GAISLER_MIGDDR2, "Xilinx MIG DDR2 Controller" },
  { GAISLER_LCDCTRL, "LCD Controller" },
  { GAISLER_SWITCHOVER, "Switchover Logic" },
  { GAISLER_FIFOUART, "UART with large FIFO" },
  { GAISLER_MUXCTRL, "Analogue multiplexer control" },
  { GAISLER_B1553BC, "AMBA Wrapper for Core1553BBC" },
  { GAISLER_B1553RT, "AMBA Wrapper for Core1553BRT" },
  { GAISLER_B1553BRM, "AMBA Wrapper for Core1553BRM" },
  { GAISLER_AES, "Advanced Encryption Standard" },
  { GAISLER_ECC, "Elliptic Curve Cryptography" },
  { GAISLER_PCIF, "AMBA Wrapper for CorePCIF" },
  { GAISLER_CLKMOD, "CPU Clock Switching Ctrl module" },
  { GAISLER_HAPSTRAK, "HAPS HapsTrak I/O Port" },
  { GAISLER_TEST_1X2, "HAPS TEST_1x2 interface" },
  { GAISLER_WILD2AHB, "WildCard CardBus interface" },
  { GAISLER_BIO1, "Basic I/O board BIO1" },
  { GAISLER_AESDMA, "AES 256 DMA" },
  { GAISLER_GRPCI2, "GRPCI2 PCI/AHB bridge" },
  { GAISLER_GRPCI2_DMA, "GRPCI2 DMA interface" },
  { GAISLER_GRPCI2_TB, "GRPCI2 Trace buffer" },
  { GAISLER_MMA, "Memory Mapped AMBA" },
  { GAISLER_SATCAN, "SatCAN controller" },
  { GAISLER_CANMUX, "CAN Bus multiplexer" },
  { GAISLER_GRTMRX, "CCSDS Telemetry Receiver" },
  { GAISLER_GRTCTX, "CCSDS Telecommand Transmitter" },
  { GAISLER_GRTMDESC, "CCSDS Telemetry Descriptor" },
  { GAISLER_GRTMVC, "CCSDS Telemetry VC Generator" },
  { GAISLER_GEFFE, "Geffe Generator" },
  { GAISLER_GPREG, "General Purpose Register" },
  { GAISLER_GRTMPAHB, "CCSDS Telemetry VC AHB Input" },
  { GAISLER_SPWCUC, "CCSDS CUC / SpaceWire I/F" },
  { GAISLER_SPW2_DMA, "GRSPW Router DMA interface" },
  { GAISLER_SPWROUTER, "GRSPW Router" },
  { GAISLER_EDCLMST, "EDCL master interface" },
  { GAISLER_GRPWTX, "PacketWire Transmitter with DMA" },
  { GAISLER_GRPWRX, "PacketWire Receiver with DMA" },
  { GAISLER_GPREGBANK, "General Purpose Register Bank" },
  { GAISLER_MIG_7SERIES, "Xilinx MIG Controller" },
  { GAISLER_GRSPW2_SIST, "GRSPW Router SIST" },
  { GAISLER_SGMII, "XILINX SGMII Interface" },
  { GAISLER_RGMII, "Gaisler RGMII Interface" },
  { GAISLER_IRQGEN, "Interrupt generator" },
  { GAISLER_GRDMAC, "GRDMAC DMA Controller" },
  { GAISLER_AHB2AVLA, "Avalon-MM memory controller" },
  { GAISLER_SPWTDP, "CCSDS TDP / SpaceWire I/F" },
  { GAISLER_L3STAT, "LEON3 Statistics Unit" },
  { GAISLER_GR740THS, "Temperature sensor" },
  { GAISLER_GRRM, "Reconfiguration Module" },
  { GAISLER_CMAP, "CCSDS Memory Access Protocol" },
  { GAISLER_CPGEN, "Discrete Command Pulse Gen" },
  { GAISLER_AMBAPROT, "AMBA Protection Unit" },
  { GAISLER_IGLOO2_BRIDGE, "Microsemi SF2/IGLOO2 MSS/HPMS" },
  { GAISLER_AHB2AXI, "AMBA AHB/AXI Bridge" },
  { GAISLER_AXI2AHB, "AMBA AXI/AHB Bridge" },
  { GAISLER_FDIR_RSTCTRL, "FDIR Reset Controller" },
  { GAISLER_APB3MST, "AHB/APB3 Bridge" },
  { GAISLER_LRAM, "Dual-port AHB(/CPU) On-Chip RAM" },
  { GAISLER_BOOTSEQ, "Custom AHB sequencer" },
  { GAISLER_TCCOP, "CCSDS Telecommand Decoder (COP)" },
  { GAISLER_SPIMASTER, "Simple SPI Master" },
  { GAISLER_SPISLAVE, "Dual-port SPI Slave" },
  { GAISLER_GRSRIO, "Serial RapidIO Logical Layer" },
  { GAISLER_AHBLM2AHB, "AHB-Lite master to AHB master" },
  { GAISLER_AHBS2NOC, "AHB slave to NoC" },
  { GAISLER_TCAU, "Authentication Unit" },
  { GAISLER_GRTMDYNVCID, "CCSDS Telemetry Dynamic VCID" },
  { GAISLER_RNOCIRQPROP, "RNoC Interrupt propagator" },
  { GAISLER_FTADDR, "DDR2/DDR3 controller with EDAC" },
  { GAISLER_ATG, "AMBA2 Test Pattern Generator" },
  { GAISLER_DFITRACE, "DFI2.1 Trace Buffer" },
  { GAISLER_SELFTEST, "TV selftest module" },
  { GAISLER_DFIERRINJ, "DFI error injection module" },
  { GAISLER_DFICHECK, "DFI timing check module" },
  { GAISLER_GRCANFD, "CAN-FD Controller with DMA" },
  { GAISLER_NIM, "Synchronous serial interface" },
  { GAISLER_GRSHYLOC, "SHYLOC Compressor with DMA" },
  { GAISLER_GRTACHOM, "Simple Digital Tachometer" },
  { GAISLER_L5STAT, "LEON5 Statistics Unit" },
  { GAISLER_LEON5, "LEON5 SPARC V8 Processor" },
  { GAISLER_LEON5DSU, "LEON5 Debug Support Unit" },
  { GAISLER_SPFI, "GRSPFI SpaceFibre Serial Link" },
  { GAISLER_RV64GC, "NOEL-V RISC-V Processor" },
  { GAISLER_RVDM, "RISC-V Debug Module" },
  { GAISLER_FTMCTRL2, "Memory controller with EDAC" },
  { GAISLER_GRDMAC2, "GRDMAC2 DMA Controller" },
  { GAISLER_GRSCRUB, "GRSCRUB FPGA Scrubber" },
  { GAISLER_GRPLIC, "RISC-V PLIC" },
  { GAISLER_CLINT, "RISC-V CLINT" },
  { GAISLER_SOCBRIDGE, "SoC to SoC bridge" },
  { GAISLER_NANDFCTRL2, "NAND Flash Controller 2" },
  { GAISLER_DARE65THS, "DARE65T Temperature sensor" },
  { GAISLER_WIZL, "GRWIZL WizardLink Serial Link" },
  { GAISLER_HSSL, "GRHSSL SpaceFibre + WizardLink" },
  { GAISLER_GRWATCHDOG, "Watchdog unit with sep clock" },
  { GAISLER_ETRACE, "RISC-V E-trace encoder" },
  { GAISLER_LEON5ADSU, "LEON5 Advanced Debug Unit" },
  { GAISLER_LEON5DMAB, "LEON5 IOMMU and DMA bridge" },
  { GAISLER_ACLINT, "RISC-V ACLINT" },
  { GAISLER_IMSIC, "RISC-V IMSIC" },
  { GAISLER_GRAPLIC, "RISC-V APLIC" },
  { GAISLER_L2CL, "L2-Cache Controller - Lite" },
  { GAISLER_L2CACHE_IO, "L2-Cache Controller I/O Port" },
  { GAISLER_AHB2AHB_STR, "AHB/AHB Stripe Bridge" },
  { GAISLER_GRIOMMURV, "RISC-V IO Memory Management Unit" },
  { GAISLER_SGMII_VCU118, "VCU118 SGMII Interface" },
  { GAISLER_APBUART_16550, "Generic UART 16550" },
  { GAISLER_IOPMP, "RISC-V IOPMP" },
  { GAISLER_EFPGA, "AHB interface to eFPGA28RH_30K" },
  { GAISLER_GRISOC_MB_EXT, "GRISoC Mailbox interface" },
  { GAISLER_GRISOC_MB_INT, "GRISoC Mailbox interface (int)" },
  { GAISLER_GRISOC_BOOT, "GRISoC Boot FSM" },
  { GAISLER_GRISOC_CRYPTO, "GRISoC Crypto accelerator" },
  { GAISLER_GRISOC_SRNG, "GRISoC Secure RNG" },
  { GAISLER_GRPPSTIMER, "PPS Timer" },
  { GAISLER_GRLSEDC, "Lattice CRAM Scrubber" },
  { GAISLER_MDIO_CTRL, "Ethernet MDIO Controller" },
  { GAISLER_OTP_WRPR, "OTP control wrapper" },
  { GAISLER_LATTICE_DDR3, "Lattice DDR3 memory controller" },
  { GAISLER_GR1553B_LRAM, "1553B Interface with local SRAM" },
  { GAISLER_N3_RVDM, "NOEL3 debug module" },
  { GAISLER_N3_RV32, "NOEL3 RISC-V Processor" },
  { GAISLER_GRETH2, "Ethernet Gbit MAC 2" },
  { GAISLER_GR765THS, "GR765 Temp/Voltage sensor" },
  { GAISLER_GR765EVCTRL, "GR765 Event controller" },
  { GAISLER_GR765SMB, "GR765 System Management Block" },
  { GAISLER_RVINTCTRL, "RISC-V Interrupt Controllers" },
  { GAISLER_RVDMEXT, "NOELV Ext. DM" },
  { GAISLER_LATTICE_DDR4, "Lattice DDR4 Memory Controller" },
  { GAISLER_GRSCRUB2, "GRSCRUB2 Multiple FPGA Scrubber" },
  { GAISLER_GR765PCIDMA, "GR765 PCIe DMA unit" },
  { GAISLER_GR765OTPIF, "GR765 OTP Interface" },
  { GAISLER_GRHSM_DMA, "GRHSM DMA iInterface" },
  { GAISLER_ST2_TX, "Supertrace2 Transmit interface" },
  { GAISLER_ST2_RX, "Supertrace2 Receive interface" },
  { GAISLER_RTM, "CCSDS Telemetry Encoder" },
  { GAISLER_APWM, "GR716 Analog PWM" },
  { GAISLER_APWM_A_CNT, "GR716 Analog PWM_A RegSync" },
  { GAISLER_APWM_AB_CNT, "GR716 Analog PWM_AB RegSync" },
  { GAISLER_APWM_CF_CNT, "GR716 Analog PWM_CF RegSync" },
  { GAISLER_APWM_TIMER32, "GR716 APWM Timer32" },
  { GAISLER_APWM_REGSYNC, "GR716 APWM RegSync32" },
  { GAISLER_APWM_PROT, "GR716 APWM ProtShdn" },
  { GAISLER_APWM_SYNC, "GR716 APWM Sync" },
  { GAISLER_APWM_FIR1, "GR716 APWM FIR1" },
  { GAISLER_APWM_CLK_DET, "GR716 APWM CLK_DET" },
  { GAISLER_APWM_FIR1_CLK, "GR716 APWM FIR1 ClkGen" },
  { GAISLER_APWM_TIMER27, "GR716 APWM Timer27 Tick gen" },
  { GAISLER_APWM_TIMESTAMP, "GR716 APWM 32 bit TimeStamp" },
  { GAISLER_RTA_DSU, "GR716 RTA Debug Support Unit" },
  { GAISLER_RTA_AHB, "GR716 RTA AHB Interface" },
  { GAISLER_BANDGAP, "GR716 Bandgap" },
  { GAISLER_MPROT, "GR716 AMBA Protection unit" },
  { GAISLER_ADC, "GR716 Analog-to-Digital Conv" },
  { GAISLER_BO, "GR716 Brownout detector" },
  { GAISLER_DAC, "ADC / DAC Interface" },
  { GAISLER_PLL, "GR716 Phase-locked loop" },
  { GAISLER_TPS, "TPS Virtual Device" },
  { GAISLER_GPIO_SEQ, "GR716 GPIO sequencer and sampler" },
  { GAISLER_SEQUENCER, "GR716 Sequencer" },
  { GAISLER_APWM_A, "GR716 Analog PWM_A" },
  { GAISLER_APWM_AB, "GR716 Analog PWM_AB" },
  { GAISLER_APWM_CF, "GR716 Analog PWM_CF" },
  { GAISLER_APWM_G, "GR716 Analog PWM_G" },
  { GAISLER_ACOMP, "GR716 Analog Comparator" },
  { GAISLER_APWM_DAC, "GR716 APWM DAC" },
  { GAISLER_GRIS, "Error Injection Controller" },
  { GAISLER_AKIDA_SUPPORT, "Akida support block" },
  { GAISLER_AHBPREFETCH, "AHB Data Prefetcher" },
  { GAISLER_GRCPI, "Camera Parallel Interface" },
  { GAISLER_DBGMST, "Bus Master Debug Interface" },
  { GAISLER_AHB_ERR_SHIM, "AHB Error Shim" },
  { 0, NULL }
};

static ambapp_device_name PENDER_devices[] = { { 0, NULL } };

static ambapp_device_name ESA_devices[] = {
  { ESA_LEON2, "LEON2 SPARC V8 Processor" },
  { ESA_LEON2APB, "LEON2 Peripheral Bus" },
  { ESA_IRQ, "LEON2 Interrupt Controller" },
  { ESA_TIMER, "Modular Timer Unit" },
  { ESA_UART, "AHB Debug UART" },
  { ESA_CFG, "LEON2 Configuration Register" },
  { ESA_IO, "General Purpose I/O port" },
  { ESA_MCTRL, "Memory controller with EDAC" },
  { ESA_PCIARB, "PCI Arbiter" },
  { ESA_HURRICANE, "HurriCANe/HurryAMBA CAN Ctrl" },
  { ESA_SPW_RMAP, "UoD/Saab SpaceWire/RMAP link" },
  { ESA_AHBUART, "AHB Debug UART" },
  { ESA_SPWA, "ESA/ASTRIUM SpaceWire link" },
  { ESA_BOSCHCAN, "SSC/BOSCH CAN Ctrl" },
  { ESA_IRQ2, "LEON2 Secondary Irq Controller" },
  { ESA_AHBSTAT, "AHB Status Register" },
  { ESA_WPROT, "LEON2 Write Protection" },
  { ESA_WPROT2, "LEON2 Extended Write Protection" },
  { ESA_PDEC3AMBA, "ESA CCSDS PDEC3AMBA TC Decoder" },
  { ESA_PTME3AMBA, "ESA CCSDS PTME3AMBA TM Encoder" },
  { 0, NULL }
};

static ambapp_device_name ASTRIUM_devices[] = { { 0, NULL } };

static ambapp_device_name OPENCHIP_devices[] = {
  { OPENCHIP_APBGPIO, "APB General Purpose IO" },
  { OPENCHIP_APBI2C, "APB I2C Interface" },
  { OPENCHIP_APBSPI, "APB SPI Interface" },
  { OPENCHIP_APBCHARLCD, "APB Character LCD" },
  { OPENCHIP_APBPWM, "APB PWM" },
  { OPENCHIP_APBPS2, "PS2 interface" },
  { OPENCHIP_APBMMCSD, "APB MMC/SD Card Interface" },
  { OPENCHIP_APBNAND, "APB NAND(SmartMedia) Interface" },
  { OPENCHIP_APBLPC, "APB LPC Interface" },
  { OPENCHIP_APBCF, "APB CompactFlash (IDE)" },
  { OPENCHIP_APBSYSACE, "APB SystemACE Interface" },
  { OPENCHIP_APB1WIRE, "APB 1-Wire Interface" },
  { OPENCHIP_APBJTAG, "APB JTAG TAP Master" },
  { OPENCHIP_APBSUI, "APB Simple User Interface" },
  { 0, NULL }
};

static ambapp_device_name OPENCORES_devices[] = { { 0, NULL } };

static ambapp_device_name CONTRIB_devices[] = {
  { CONTRIB_CORE1, "Contributed core 1" },
  { CONTRIB_CORE2, "Contributed core 2" },
  { CONTRIB_CORE3, "Contributed core 3" },
  { 0, NULL }
};

static ambapp_device_name DLR_devices[] = { { 0, NULL } };

static ambapp_device_name EONIC_devices[] = { { 0, NULL } };

static ambapp_device_name TELECOMPT_devices[] = { { 0, NULL } };

static ambapp_device_name DTU_devices[] = {
  { DTU_IV, "Instrument Virtualizer" },
  { DTU_RBMMTRANS, "RB/MM Transfer" },
  { DTU_FTMCTRL, "Memory controller with EDAC" },
  { 0, NULL }
};

static ambapp_device_name BSC_devices[] = {
  { BSC_CORE1, "Contributed core 1" },
  { BSC_CORE2, "Contributed core 2" },
  { BSC_PMU, "AHB Performance Monitoring Unit" },
  { BSC_SAFESU, "SafeSU - AHB Statistics Unit" },
  { 0, NULL }
};

static ambapp_device_name RADIONOR_devices[] = { { 0, NULL } };

static ambapp_device_name GLEICHMANN_devices[] = {
  { GLEICHMANN_CUSTOM, "Custom device" },
  { GLEICHMANN_GEOLCD01, "GEOLCD01 graphics system" },
  { GLEICHMANN_DAC, "ADC / DAC Interface" },
  { GLEICHMANN_HPI, "AHB-to-HPI bridge" },
  { GLEICHMANN_SPI, "APB SPI Interface" },
  { GLEICHMANN_HIFC, "Human interface controller" },
  { GLEICHMANN_ADCDAC, "ADC / DAC Interface" },
  { GLEICHMANN_SPIOC, "SPI master for SDCard IF" },
  { GLEICHMANN_AC97, "AC97 Controller" },
  { 0, NULL }
};

static ambapp_device_name MENTA_devices[] = {
  { MENTA_EFPGA_IP, "eFPGA Core IP" },
  { 0, NULL }
};

static ambapp_device_name SUN_devices[] = {
  { SUN_T1, "Niagara T1 SPARC V9 Processor" },
  { SUN_S1, "Niagara S1 SPARC V9 Processor" },
  { 0, NULL }
};

static ambapp_device_name MOVIDIA_devices[] = { { 0, NULL } };

static ambapp_device_name L3T_devices[] = { { 0, NULL } };

static ambapp_device_name ORBITA_devices[] = {
  { ORBITA_1553B, "MIL-STD-1553B Interface" },
  { ORBITA_429, "429 Interface" },
  { ORBITA_SPI, "APB SPI Interface" },
  { ORBITA_I2C, "APB I2C Interface" },
  { ORBITA_SMARTCARD, "Smart Card Reader" },
  { ORBITA_SDCARD, "SD Card Reader" },
  { ORBITA_UART16550, "16550 UART" },
  { ORBITA_CRYPTO, "GRISoC Crypto accelerator" },
  { ORBITA_SYSIF, "System Interface" },
  { ORBITA_PIO, "General Purpose I/O port" },
  { ORBITA_RTC, "CCSDS Telecommand Decoder" },
  { ORBITA_COLORLCD, "Color LCD Controller" },
  { ORBITA_PCI, "PCI Module" },
  { ORBITA_DSP, "DPS Co-Processor" },
  { ORBITA_USBHOST, "USB Host" },
  { ORBITA_USBDEV, "USB Device" },
  { 0, NULL }
};

static ambapp_device_name SIEMENS_devices[] = {
  { SIEMENS_COMMON_REG, "Common Register" },
  { SIEMENS_HDLC, "HDLC Controller" },
  { SIEMENS_CAP, "Capture Counter Unit" },
  { SIEMENS_AMBA_MONITOR, "AMBA Monitor" },
  { SIEMENS_COMLINK, "Comlink Controller" },
  { SIEMENS_OSOP, "OSOP Link Controller" },
  { SIEMENS_PCI_BRIDGE, "PCI Bridge" },
  { SIEMENS_LINE_CODING_UNIT, "Linecoding Unit" },
  { SIEMENS_AHBRAM, "Single-port AHB SRAM module" },
  { SIEMENS_EHD_SWITCH, "EHD Switch" },
  { SIEMENS_UART, "AHB Debug UART" },
  { 0, NULL }
};

static ambapp_device_name ULTRASOC_devices[] = {
  { ULTRASOC_ETRACE_AXI_COM, "E-Trace AXI Communicator" },
  { ULTRASOC_ETRACE_SMB_COM, "E-Trace SMB Communicator" },
  { 0, NULL }
};

static ambapp_device_name SYNOPSYS_devices[] = {
  { SYNOPSYS_PCIE_CTRL, "PCI Express controller" },
  { 0, NULL }
};

static ambapp_device_name NASA_devices[] = {
  { NASA_EP32, "EP32 Forth processor" },
  { 0, NULL }
};

static ambapp_device_name NIIET_devices[] = { { 0, NULL } };

static ambapp_device_name JHUAPL_SRI_devices[] = { { 0, NULL } };

static ambapp_device_name JHUAPL_SEE_devices[] = { { 0, NULL } };

static ambapp_device_name JHUAPL_SER_devices[] = { { 0, NULL } };

static ambapp_device_name JHUAPL_SES_devices[] = { { 0, NULL } };

static ambapp_device_name SEMIBLOCKS_devices[] = { { 0, NULL } };

static ambapp_device_name NEC_devices[] = { { 0, NULL } };

static ambapp_device_name HONEYWELL_devices[] = { { 0, NULL } };

static ambapp_device_name ST_devices[] = {
  { ST_MIPHY, "MIPHY transceiver" },
  { ST_AIB, "AIB (die2die)" },
  { ST_PMB, "Process monitor block" },
  { 0, NULL }
};

static ambapp_device_name AITECH_devices[] = { { 0, NULL } };

static ambapp_device_name S3_devices[] = { { 0, NULL } };

static ambapp_device_name TTTECH_devices[] = {
  { TTTECH_SWITCH, "EHD Switch" },
  { 0, NULL }
};

static ambapp_device_name TAS_devices[] = {
  { TAS_HOUSE_KEEPING_ADC, "House-Keeping ADC" },
  { 0, NULL }
};

static ambapp_device_name RECORE_devices[] = {
  { RECORE_PROC_SUB_BRDG, "Processing Subsystem Bridge" },
  { RECORE_PROC_SUB_DBG, "Processing Subsystem Debug" },
  { RECORE_XENTIUM_CORE, "Xentium DSP IP core" },
  { RECORE_XENTIUM_DEBUG, "Xentium DSP Debug Support Unit" },
  { RECORE_XENTIUM_ICACHE, "Xentium iCache AHB master i/f" },
  { RECORE_XENTIUM_DEVICE, "Xentium devices" },
  { 0, NULL }
};

static ambapp_device_name AAC_devices[] = {
  { AAC_MEMCTRL, "SDRAM Memory controller" },
  { AAC_SOCINFO, "System-on-Chip info" },
  { AAC_GPIO, "General Purpose I/O port" },
  { AAC_WDT, "Watchdog timer" },
  { AAC_NVRAM, "Non-volatile RAM" },
  { AAC_ERRMAN, "Error manager" },
  { AAC_SCET, "SpaceCraft Elapsed Timer" },
  { AAC_NANDFLASH, "System Flash for boot images" },
  { AAC_ADC, "GR716 Analog-to-Digital Conv" },
  { AAC_SPW, "SpaceWire Serial Link" },
  { AAC_MM, "Mass memory flash with DMA" },
  { AAC_CCSDS, "CCSDS TM/TC with DMA" },
  { AAC_UART, "AHB Debug UART" },
  { AAC_I2C, "APB I2C Interface" },
  { AAC_ETHMAC, "GR Ethernet MAC" },
  { AAC_CAN, "ESA HurriCANe CAN with DMA" },
  { AAC_USB, "USB slave" },
  { AAC_SPI, "APB SPI Interface" },
  { AAC_CUSTOM1, "Custom IP core type 1" },
  { AAC_CUSTOM2, "Custom IP core type 2" },
  { AAC_CUSTOM3, "Custom IP core type 3" },
  { 0, NULL }
};

static ambapp_device_name UC_BERKELEY_devices[] = { { 0, NULL } };

static ambapp_device_name ACTEL_devices[] = {
  { ACTEL_COREMP7, "CoreMP7 Processor" },
  { ACTEL_RTG4FDDRCE, "RTG4 FDDR East Controller" },
  { ACTEL_RTG4FDDRCW, "RTG4 FDDR West Controller" },
  { ACTEL_IGLOO2_BRIDGE, "Microsemi SF2/IGLOO2 MSS/HPMS" },
  { ACTEL_MDDR, "MDDR Bridge" },
  { ACTEL_APB3SLV, "Generic APB3 Slave Interface" },
  { ACTEL_SERDES, "SERDES Interface" },
  { ACTEL_FICSLV, "FIC Slave Wrapper" },
  { ACTEL_FICMST, "FIC Master Wrapper" },
  { ACTEL_RTG4SERDES, "RTG4 SERDES Interface" },
  { ACTEL_PFFDDR3, "PolarFire FDDR3 Controller" },
  { ACTEL_PFFDDR4, "PolarFire FDDR4 Controller" },
  { 0, NULL }
};

static ambapp_device_name BG_devices[] = {
  { BG_RIXCORE, "RIXCore SF GNSS Processor" },
  { BG_DIAGIF, "Diagnosis I/F" },
  { BG_WDOG, "WDOG" },
  { BG_SPI, "APB SPI Interface" },
  { 0, NULL }
};

static ambapp_device_name APPLECORE_devices[] = {
  { APPLECORE_UTLEON3, "AppleCore uT-LEON3 Processor" },
  { APPLECORE_UTLEON3DSU, "AppleCore uT-LEON3 DSU" },
  { APPLECORE_APBPERFCNT, "APBPERFCNT" },
  { 0, NULL }
};

static ambapp_device_name C3E_devices[] = { { 0, NULL } };

static ambapp_device_name CBKPAN_devices[] = {
  { CBKPAN_FTNANDCTRL, "NAND FLASH controller w/DMA" },
  { CBKPAN_FTEEPROMCTRL, "Fault Toler. EEPROM Controller" },
  { CBKPAN_FTSDCTRL16, "Fault Toler. 16-bit SDRAM Ctrl." },
  { CBKPAN_STIXCTRL, "SolO/STIX IDPU dedicated ctrl." },
  { 0, NULL }
};

static ambapp_device_name CAL_devices[] = {
  { CAL_DDRCTRL, "DDRCTRL" },
  { 0, NULL }
};

static ambapp_device_name CETON_devices[] = { { 0, NULL } };

static ambapp_device_name SSTL_devices[] = {
  { SSTL_HDLC, "HDLC Controller" },
  { SSTL_INICAN, "iniCAN Controller" },
  { SSTL_ZERO_MEM, "DDR3 Zero initialiser" },
  { 0, NULL }
};

static ambapp_device_name EMBEDDIT_devices[] = { { 0, NULL } };

static ambapp_device_name BRAINCHIP_devices[] = {
  { BRAINCHIP_AKIDA, "Brainchip Akida Accelerator" },
  { BRAINCHIP_AXI2AHB, "AMBA AXI/AHB Bridge" },
  { 0, NULL }
};

static ambapp_device_name MAGICS_devices[] = {
  { MAGICS_MOVIQ, "Magics MOVIQ Accelerator" },
  { 0, NULL }
};

static ambapp_device_name LEAT_devices[] = {
  { LEAT_SPLEAT, "LEAT SPLEAT Accelerator" },
  { 0, NULL }
};

static ambapp_device_name NASA_GSFC_devices[] = { { 0, NULL } };

static ambapp_device_name AZST_devices[] = { { 0, NULL } };

static ambapp_vendor_devnames vendors[] = {
  { VENDOR_GAISLER, "GAISLER", GAISLER_devices },
  { VENDOR_PENDER, "PENDER", PENDER_devices },
  { VENDOR_ESA, "ESA", ESA_devices },
  { VENDOR_ASTRIUM, "ASTRIUM", ASTRIUM_devices },
  { VENDOR_OPENCHIP, "OPENCHIP", OPENCHIP_devices },
  { VENDOR_OPENCORES, "OPENCORES", OPENCORES_devices },
  { VENDOR_CONTRIB, "CONTRIB", CONTRIB_devices },
  { VENDOR_DLR, "DLR", DLR_devices },
  { VENDOR_EONIC, "EONIC", EONIC_devices },
  { VENDOR_TELECOMPT, "TELECOMPT", TELECOMPT_devices },
  { VENDOR_DTU, "DTU", DTU_devices },
  { VENDOR_BSC, "BSC", BSC_devices },
  { VENDOR_RADIONOR, "RADIONOR", RADIONOR_devices },
  { VENDOR_GLEICHMANN, "GLEICHMANN", GLEICHMANN_devices },
  { VENDOR_MENTA, "MENTA", MENTA_devices },
  { VENDOR_SUN, "SUN", SUN_devices },
  { VENDOR_MOVIDIA, "MOVIDIA", MOVIDIA_devices },
  { VENDOR_L3T, "L3T", L3T_devices },
  { VENDOR_ORBITA, "ORBITA", ORBITA_devices },
  { VENDOR_SIEMENS, "SIEMENS", SIEMENS_devices },
  { VENDOR_ULTRASOC, "ULTRASOC", ULTRASOC_devices },
  { VENDOR_SYNOPSYS, "SYNOPSYS", SYNOPSYS_devices },
  { VENDOR_NASA, "NASA", NASA_devices },
  { VENDOR_NIIET, "NIIET", NIIET_devices },
  { VENDOR_JHUAPL_SRI, "JHUAPL_SRI", JHUAPL_SRI_devices },
  { VENDOR_JHUAPL_SEE, "JHUAPL_SEE", JHUAPL_SEE_devices },
  { VENDOR_JHUAPL_SER, "JHUAPL_SER", JHUAPL_SER_devices },
  { VENDOR_JHUAPL_SES, "JHUAPL_SES", JHUAPL_SES_devices },
  { VENDOR_SEMIBLOCKS, "SEMIBLOCKS", SEMIBLOCKS_devices },
  { VENDOR_NEC, "NEC", NEC_devices },
  { VENDOR_HONEYWELL, "HONEYWELL", HONEYWELL_devices },
  { VENDOR_ST, "ST", ST_devices },
  { VENDOR_AITECH, "AITECH", AITECH_devices },
  { VENDOR_S3, "S3", S3_devices },
  { VENDOR_TTTECH, "TTTECH", TTTECH_devices },
  { VENDOR_TAS, "TAS", TAS_devices },
  { VENDOR_RECORE, "RECORE", RECORE_devices },
  { VENDOR_AAC, "AAC", AAC_devices },
  { VENDOR_UC_BERKELEY, "UC_BERKELEY", UC_BERKELEY_devices },
  { VENDOR_ACTEL, "ACTEL", ACTEL_devices },
  { VENDOR_BG, "BG", BG_devices },
  { VENDOR_APPLECORE, "APPLECORE", APPLECORE_devices },
  { VENDOR_C3E, "C3E", C3E_devices },
  { VENDOR_CBKPAN, "CBKPAN", CBKPAN_devices },
  { VENDOR_CAL, "CAL", CAL_devices },
  { VENDOR_CETON, "CETON", CETON_devices },
  { VENDOR_SSTL, "SSTL", SSTL_devices },
  { VENDOR_EMBEDDIT, "EMBEDDIT", EMBEDDIT_devices },
  { VENDOR_BRAINCHIP, "BRAINCHIP", BRAINCHIP_devices },
  { VENDOR_MAGICS, "MAGICS", MAGICS_devices },
  { VENDOR_LEAT, "LEAT", LEAT_devices },
  { VENDOR_NASA_GSFC, "NASA_GSFC", NASA_GSFC_devices },
  { VENDOR_AZST, "AZST", AZST_devices },
  { 0, NULL, NULL }
};
/*****************************************************************/

static char *ambapp_get_devname( ambapp_device_name *devs, int id )
{
  while ( devs->device_id > 0 ) {
    if ( devs->device_id == id ) {
      return devs->name;
    }
    devs++;
  }
  return NULL;
}

char *ambapp_device_id2str( unsigned int vendor, int id )
{
  ambapp_vendor_devnames *ven = &vendors[ 0 ];

  while ( ven->vendor_id > 0 ) {
    if ( ven->vendor_id == vendor ) {
      return ambapp_get_devname( ven->devices, id );
    }
    ven++;
  }
  return NULL;
}

char *ambapp_vendor_id2str( unsigned int vendor )
{
  ambapp_vendor_devnames *ven = &vendors[ 0 ];

  while ( ven->vendor_id > 0 ) {
    if ( ven->vendor_id == vendor ) {
      return ven->name;
    }
    ven++;
  }
  return NULL;
}

int ambapp_vendev_id2str( unsigned int vendor, int id, char *buf )
{
  char *dstr, *vstr;

  *buf = '\0';

  vstr = ambapp_vendor_id2str( vendor );
  if ( vstr == NULL ) {
    return 0;
  }

  dstr = ambapp_device_id2str( vendor, id );
  if ( dstr == NULL ) {
    return 0;
  }

  strcpy( buf, vstr );
  strcat( buf, "_" );
  strcat( buf, dstr );

  return strlen( buf );
}
