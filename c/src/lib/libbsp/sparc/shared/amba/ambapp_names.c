/*
 *  AMBA Plug & Play Device and Vendor name database: Created from GRLIB 3386.
 *
 *  COPYRIGHT (c) 2009.
 *  Aeroflex Gaisler.
 *
 *  The device and vendor definitions are extracted with a script from
 *  GRLIB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <ambapp_ids.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

typedef struct {
  int device_id;
  char *name;
} ambapp_device_name;

typedef struct {
  unsigned int vendor_id;
  char *name;
  ambapp_device_name *devices;
} ambapp_vendor_devnames;

/**************** AUTO GENERATED FROM devices.vhd ****************/
static ambapp_device_name GAISLER_devices[] =
{
  {GAISLER_LEON2DSU, "LEON2DSU"},
  {GAISLER_LEON3, "LEON3"},
  {GAISLER_LEON3DSU, "LEON3DSU"},
  {GAISLER_ETHAHB, "ETHAHB"},
  {GAISLER_APBMST, "APBMST"},
  {GAISLER_AHBUART, "AHBUART"},
  {GAISLER_SRCTRL, "SRCTRL"},
  {GAISLER_SDCTRL, "SDCTRL"},
  {GAISLER_SSRCTRL, "SSRCTRL"},
  {GAISLER_APBUART, "APBUART"},
  {GAISLER_IRQMP, "IRQMP"},
  {GAISLER_AHBRAM, "AHBRAM"},
  {GAISLER_AHBDPRAM, "AHBDPRAM"},
  {GAISLER_GPTIMER, "GPTIMER"},
  {GAISLER_PCITRG, "PCITRG"},
  {GAISLER_PCISBRG, "PCISBRG"},
  {GAISLER_PCIFBRG, "PCIFBRG"},
  {GAISLER_PCITRACE, "PCITRACE"},
  {GAISLER_DMACTRL, "DMACTRL"},
  {GAISLER_AHBTRACE, "AHBTRACE"},
  {GAISLER_DSUCTRL, "DSUCTRL"},
  {GAISLER_CANAHB, "CANAHB"},
  {GAISLER_GPIO, "GPIO"},
  {GAISLER_AHBROM, "AHBROM"},
  {GAISLER_AHBJTAG, "AHBJTAG"},
  {GAISLER_ETHMAC, "ETHMAC"},
  {GAISLER_SWNODE, "SWNODE"},
  {GAISLER_SPW, "SPW"},
  {GAISLER_AHB2AHB, "AHB2AHB"},
  {GAISLER_USBDC, "USBDC"},
  {GAISLER_USB_DCL, "USB_DCL"},
  {GAISLER_DDRMP, "DDRMP"},
  {GAISLER_ATACTRL, "ATACTRL"},
  {GAISLER_DDRSP, "DDRSP"},
  {GAISLER_EHCI, "EHCI"},
  {GAISLER_UHCI, "UHCI"},
  {GAISLER_I2CMST, "I2CMST"},
  {GAISLER_SPW2, "SPW2"},
  {GAISLER_AHBDMA, "AHBDMA"},
  {GAISLER_NUHOSP3, "NUHOSP3"},
  {GAISLER_CLKGATE, "CLKGATE"},
  {GAISLER_SPICTRL, "SPICTRL"},
  {GAISLER_DDR2SP, "DDR2SP"},
  {GAISLER_SLINK, "SLINK"},
  {GAISLER_GRTM, "GRTM"},
  {GAISLER_GRTC, "GRTC"},
  {GAISLER_GRPW, "GRPW"},
  {GAISLER_GRCTM, "GRCTM"},
  {GAISLER_GRHCAN, "GRHCAN"},
  {GAISLER_GRFIFO, "GRFIFO"},
  {GAISLER_GRADCDAC, "GRADCDAC"},
  {GAISLER_GRPULSE, "GRPULSE"},
  {GAISLER_GRTIMER, "GRTIMER"},
  {GAISLER_AHB2PP, "AHB2PP"},
  {GAISLER_GRVERSION, "GRVERSION"},
  {GAISLER_APB2PW, "APB2PW"},
  {GAISLER_PW2APB, "PW2APB"},
  {GAISLER_GRCAN, "GRCAN"},
  {GAISLER_I2CSLV, "I2CSLV"},
  {GAISLER_U16550, "U16550"},
  {GAISLER_AHBMST_EM, "AHBMST_EM"},
  {GAISLER_AHBSLV_EM, "AHBSLV_EM"},
  {GAISLER_GRTESTMOD, "GRTESTMOD"},
  {GAISLER_ASCS, "ASCS"},
  {GAISLER_IPMVBCTRL, "IPMVBCTRL"},
  {GAISLER_SPIMCTRL, "SPIMCTRL"},
  {GAISLER_LEON4, "LEON4"},
  {GAISLER_LEON4DSU, "LEON4DSU"},
  {GAISLER_GRPWM, "GRPWM"},
  {GAISLER_FTAHBRAM, "FTAHBRAM"},
  {GAISLER_FTSRCTRL, "FTSRCTRL"},
  {GAISLER_AHBSTAT, "AHBSTAT"},
  {GAISLER_LEON3FT, "LEON3FT"},
  {GAISLER_FTMCTRL, "FTMCTRL"},
  {GAISLER_FTSDCTRL, "FTSDCTRL"},
  {GAISLER_FTSRCTRL8, "FTSRCTRL8"},
  {GAISLER_MEMSCRUB, "MEMSCRUB"},
  {GAISLER_APBPS2, "APBPS2"},
  {GAISLER_VGACTRL, "VGACTRL"},
  {GAISLER_LOGAN, "LOGAN"},
  {GAISLER_SVGACTRL, "SVGACTRL"},
  {GAISLER_T1AHB, "T1AHB"},
  {GAISLER_MP7WRAP, "MP7WRAP"},
  {GAISLER_GRSYSMON, "GRSYSMON"},
  {GAISLER_GRACECTRL, "GRACECTRL"},
  {GAISLER_B1553BC, "B1553BC"},
  {GAISLER_B1553RT, "B1553RT"},
  {GAISLER_B1553BRM, "B1553BRM"},
  {GAISLER_SATCAN, "SATCAN"},
  {GAISLER_CANMUX, "CANMUX"},
  {GAISLER_GRTMRX, "GRTMRX"},
  {GAISLER_GRTCTX, "GRTCTX"},
  {GAISLER_GRTMDESC, "GRTMDESC"},
  {GAISLER_GRTMVC, "GRTMVC"},
  {GAISLER_GEFFE, "GEFFE"},
  {GAISLER_AES, "AES"},
  {GAISLER_GRAESDMA, "GRAESDMA"},
  {GAISLER_ECC, "ECC"},
  {GAISLER_PCIF, "PCIF"},
  {GAISLER_CLKMOD, "CLKMOD"},
  {GAISLER_HAPSTRAK, "HAPSTRAK"},
  {GAISLER_TEST_1X2, "TEST_1X2"},
  {GAISLER_WILD2AHB, "WILD2AHB"},
  {GAISLER_BIO1, "BIO1"},
  {GAISLER_GR1553B, "GR1553B"},
  {GAISLER_L2CACHE, "L2CACHE"},
  {GAISLER_L4STAT, "L4STAT"},
  {GAISLER_GRPCI2, "GRPCI2"},
  {GAISLER_GRPCI2_DMA, "GRPCI2_DMA"},
  {GAISLER_GRIOMMU, "GRIOMMU"},
  {GAISLER_SPW2_DMA, "SPW2_DMA"},
  {GAISLER_SPW_ROUTER, "SPW_ROUTER"},
  {0, NULL}
};

static ambapp_device_name PENDER_devices[] =
{
  {0, NULL}
};

static ambapp_device_name ESA_devices[] =
{
  {ESA_LEON2, "LEON2"},
  {ESA_LEON2APB, "LEON2APB"},
  {ESA_IRQ, "IRQ"},
  {ESA_TIMER, "TIMER"},
  {ESA_UART, "UART"},
  {ESA_CFG, "CFG"},
  {ESA_IO, "IO"},
  {ESA_MCTRL, "MCTRL"},
  {ESA_PCIARB, "PCIARB"},
  {ESA_HURRICANE, "HURRICANE"},
  {ESA_SPW_RMAP, "SPW_RMAP"},
  {ESA_AHBUART, "AHBUART"},
  {ESA_SPWA, "SPWA"},
  {ESA_BOSCHCAN, "BOSCHCAN"},
  {ESA_IRQ2, "IRQ2"},
  {ESA_AHBSTAT, "AHBSTAT"},
  {ESA_WPROT, "WPROT"},
  {ESA_WPROT2, "WPROT2"},
  {ESA_PDEC3AMBA, "PDEC3AMBA"},
  {ESA_PTME3AMBA, "PTME3AMBA"},
  {0, NULL}
};

static ambapp_device_name ASTRIUM_devices[] =
{
  {0, NULL}
};

static ambapp_device_name OPENCHIP_devices[] =
{
  {OPENCHIP_APBGPIO, "APBGPIO"},
  {OPENCHIP_APBI2C, "APBI2C"},
  {OPENCHIP_APBSPI, "APBSPI"},
  {OPENCHIP_APBCHARLCD, "APBCHARLCD"},
  {OPENCHIP_APBPWM, "APBPWM"},
  {OPENCHIP_APBPS2, "APBPS2"},
  {OPENCHIP_APBMMCSD, "APBMMCSD"},
  {OPENCHIP_APBNAND, "APBNAND"},
  {OPENCHIP_APBLPC, "APBLPC"},
  {OPENCHIP_APBCF, "APBCF"},
  {OPENCHIP_APBSYSACE, "APBSYSACE"},
  {OPENCHIP_APB1WIRE, "APB1WIRE"},
  {OPENCHIP_APBJTAG, "APBJTAG"},
  {OPENCHIP_APBSUI, "APBSUI"},
  {0, NULL}
};

static ambapp_device_name OPENCORES_devices[] =
{
  {0, NULL}
};

static ambapp_device_name CONTRIB_devices[] =
{
  {CONTRIB_CORE1, "CORE1"},
  {CONTRIB_CORE2, "CORE2"},
  {0, NULL}
};

static ambapp_device_name EONIC_devices[] =
{
  {0, NULL}
};

static ambapp_device_name RADIONOR_devices[] =
{
  {0, NULL}
};

static ambapp_device_name GLEICHMANN_devices[] =
{
  {GLEICHMANN_CUSTOM, "CUSTOM"},
  {GLEICHMANN_GEOLCD01, "GEOLCD01"},
  {GLEICHMANN_DAC, "DAC"},
  {GLEICHMANN_HPI, "HPI"},
  {GLEICHMANN_SPI, "SPI"},
  {GLEICHMANN_HIFC, "HIFC"},
  {GLEICHMANN_ADCDAC, "ADCDAC"},
  {GLEICHMANN_SPIOC, "SPIOC"},
  {GLEICHMANN_AC97, "AC97"},
  {0, NULL}
};

static ambapp_device_name MENTA_devices[] =
{
  {0, NULL}
};

static ambapp_device_name SUN_devices[] =
{
  {SUN_T1, "SUN_T1"},
  {SUN_S1, "SUN_S1"},
  {0, NULL}
};

static ambapp_device_name MOVIDIA_devices[] =
{
  {0, NULL}
};

static ambapp_device_name ORBITA_devices[] =
{
  {ORBITA_1553B, "1553B"},
  {ORBITA_429, "429"},
  {ORBITA_SPI, "SPI"},
  {ORBITA_I2C, "I2C"},
  {ORBITA_SMARTCARD, "SMARTCARD"},
  {ORBITA_SDCARD, "SDCARD"},
  {ORBITA_UART16550, "UART16550"},
  {ORBITA_CRYPTO, "CRYPTO"},
  {ORBITA_SYSIF, "SYSIF"},
  {ORBITA_PIO, "PIO"},
  {ORBITA_RTC, "RTC"},
  {ORBITA_COLORLCD, "COLORLCD"},
  {ORBITA_PCI, "PCI"},
  {ORBITA_DSP, "DSP"},
  {ORBITA_USBHOST, "USBHOST"},
  {ORBITA_USBDEV, "USBDEV"},
  {0, NULL}
};

static ambapp_device_name SYNOPSYS_devices[] =
{
  {0, NULL}
};

static ambapp_device_name NASA_devices[] =
{
  {NASA_EP32, "EP32"},
  {0, NULL}
};

static ambapp_device_name CAL_devices[] =
{
  {CAL_DDRCTRL, "DDRCTRL"},
  {0, NULL}
};

static ambapp_device_name EMBEDDIT_devices[] =
{
  {0, NULL}
};

static ambapp_device_name CETON_devices[] =
{
  {0, NULL}
};

static ambapp_device_name ACTEL_devices[] =
{
  {ACTEL_COREMP7, "COREMP7"},
  {0, NULL}
};

static ambapp_vendor_devnames vendors[] =
{
  {VENDOR_GAISLER, "GAISLER", GAISLER_devices},
  {VENDOR_PENDER, "PENDER", PENDER_devices},
  {VENDOR_ESA, "ESA", ESA_devices},
  {VENDOR_ASTRIUM, "ASTRIUM", ASTRIUM_devices},
  {VENDOR_OPENCHIP, "OPENCHIP", OPENCHIP_devices},
  {VENDOR_OPENCORES, "OPENCORES", OPENCORES_devices},
  {VENDOR_CONTRIB, "CONTRIB", CONTRIB_devices},
  {VENDOR_EONIC, "EONIC", EONIC_devices},
  {VENDOR_RADIONOR, "RADIONOR", RADIONOR_devices},
  {VENDOR_GLEICHMANN, "GLEICHMANN", GLEICHMANN_devices},
  {VENDOR_MENTA, "MENTA", MENTA_devices},
  {VENDOR_SUN, "SUN", SUN_devices},
  {VENDOR_MOVIDIA, "MOVIDIA", MOVIDIA_devices},
  {VENDOR_ORBITA, "ORBITA", ORBITA_devices},
  {VENDOR_SYNOPSYS, "SYNOPSYS", SYNOPSYS_devices},
  {VENDOR_NASA, "NASA", NASA_devices},
  {VENDOR_CAL, "CAL", CAL_devices},
  {VENDOR_EMBEDDIT, "EMBEDDIT", EMBEDDIT_devices},
  {VENDOR_CETON, "CETON", CETON_devices},
  {VENDOR_ACTEL, "ACTEL", ACTEL_devices},
  {0, NULL, NULL}
};

/*****************************************************************/

static char *ambapp_get_devname(ambapp_device_name *devs, int id)
{
  while (devs->device_id > 0) {
    if (devs->device_id == id)
      return devs->name;
    devs++;
  }
  return NULL;
}

char *ambapp_device_id2str(int vendor, int id)
{
  ambapp_vendor_devnames *ven = &vendors[0];

  while (ven->vendor_id > 0) {
    if (ven->vendor_id == vendor)
      return ambapp_get_devname(ven->devices, id);
    ven++;
  }
  return NULL;
}

char *ambapp_vendor_id2str(int vendor)
{
  ambapp_vendor_devnames *ven = &vendors[0];

  while (ven->vendor_id > 0) {
    if (ven->vendor_id == vendor)
      return ven->name;
    ven++;
  }
  return NULL;
}

int ambapp_vendev_id2str(int vendor, int id, char *buf)
{
  char *dstr, *vstr;

  *buf = '\0';

  vstr = ambapp_vendor_id2str(vendor);
  if (vstr == NULL)
    return 0;

  dstr = ambapp_device_id2str(vendor, id);
  if (dstr == NULL)
    return 0;

  strcpy(buf, vstr);
  strcat(buf, "_");
  strcat(buf, dstr);

  return strlen(buf);
}
