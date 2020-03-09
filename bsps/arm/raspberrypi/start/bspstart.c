/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Raspberry pi startup code.
 */

/*
 * Copyright (c) 2013 by Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/stackalloc.h>
#include <bsp/raspberrypi.h>
#include <bsp/vc.h>
#include <bsp/fdt.h>

#include <libfdt.h>

static const struct {
    uint32_t code;
    const char* label;
} rpi_codes[] =
{
    { 0x900021, "A+ 1.1 (512MB)" },
    { 0x900032, "B+ 1.2 (512MB)" },
    { 0x900092, "Zero 1.2 (512MB)" },
    { 0x900093, "Zero 1.3 (512MB)" },
    { 0x9000c1, "Zero W 1.1 (512MB)" },
    { 0x9020e0, "3A+ 1.0 (512MB)" },
    { 0x920092, "Zero 1.2 (512MB)" },
    { 0x920093, "Zero 1.3 (512MB)" },
    { 0x900061, "CM 1.1 (512MB)" },
    { 0xa01040, "2B 1.0 (1GB)" },
    { 0xa01041, "2B 1.1 (1GB)" },
    { 0xa02082, "3B 1.2 (1GB)" },
    { 0xa020a0, "CM3 1.0 (1GB)" },
    { 0xa020d3, "3B+ 1.3 (1GB)" },
    { 0xa21041, "2B 1.1 (1GB)" },
    { 0xa22042, "2B 1,2 (with BCM2837) (1GB)" },
    { 0xa22082, "3B 1.2 (1GB)" },
    { 0xa220a0, "CM3 1.0 (1GB)" },
    { 0xa32082, "3B 1.2 (1GB)" },
    { 0xa52082, "3B 1.2 (1GB)" },
    { 0xa22083, "3B 1.3 (1GB)" },
    { 0xa02100, "CM3+ 1.0 (1GB)" },
    { 0xa03111, "4B 1.1 (1GB)" },
    { 0xb03111, "4B 1.1 (2GB)" },
    { 0xc03111, "4B 1.1 (4GB)" },
};

static const char* rpi_types[] = {
    "A",
    "B",
    "A+",
    "B+",
    "2B",
    "Alpha (early prototype)",
    "CM1",
    "3B",
    "Zero",
    "CM3",
    "Zero W",
    "3B+",
    "3A+",
    "Internal use only",
    "CM3+",
    "4B",
};

static const char* rpi_mem[] =
{
    "256MB",
    "512MB",
    "1GB",
    "2GB",
    "4GB"
};

#define NUMOF(_s) (sizeof(_s) / sizeof(_s[0]))

void *raspberrypi_get_reg_of_node(const void *fdt, int node)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "reg", &len);
  if (val == NULL || len < 4) {
    return NULL;
  }

  return (BUS_TO_PHY((void *) fdt32_to_cpu(val[0])));
}

#ifdef BSP_FDT_IS_SUPPORTED
uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  uint32_t controller = intr[0];
  uint32_t source = intr[1];

  switch ( controller ) {
    case 0:
        return source + BCM2835_IRQ_ID_BASIC_BASE_ID;
        break;
    case 1:
        return source + BCM2835_IRQ_SET1_MIN;
        break;
    case 2:
        return source + BCM2835_IRQ_SET2_MIN;
        break;
    default:
        return BSP_INTERRUPT_VECTOR_INVALID;
        break;
  }
}
#endif /* BSP_FDT_IS_SUPPORTED */

void bsp_start(void)
{
    bcm2835_get_board_spec_entries spec = { 0 };

    printk("\nRTEMS RPi ");
    if (bcm2835_mailbox_get_board_revision( &spec ) >= 0) {
	size_t i;
	for (i = 0; i < NUMOF(rpi_codes); ++i) {
	    if (rpi_codes[i].code == spec.spec) {
		printk("%s [%08x]", rpi_codes[i].label, spec.spec);
		break;
	    }
	}
	if (i >= NUMOF(rpi_codes)) {
	    uint32_t type = (spec.spec >> 4) & 0xff;
	    uint32_t mem = (spec.spec >> (4 + 4 + 8 + 4)) & 0xf;
	    printk(" unknown code [%08x] ", spec.spec);
	    if (type < NUMOF(rpi_types))
		printk(rpi_types[type]);
	    else
		printk("type: %02x", type);
	    if (mem < NUMOF(rpi_mem))
		printk(" %s", rpi_mem[mem]);
	    else
		printk(" mem: %x", mem);
	}
	printk("\n");
    }
    else {
	printk(": ERROR reading mailbox\n");
    }

    bsp_interrupt_initialize();
}
