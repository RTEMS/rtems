/*
 *  $Id$
 */

/* Memory map for the pmc901 ; works for most RAMiX PMC/CPCI modules as well */

#define UART_REG 0xc0000100
#define LED_REG  0xc0000200
#define DRAM_BASE 0xa0000000
#define DRAM_SIZE 0x00400000
#define DRAM_SIZE_MASK 0xffc00000
#define	FLASH_BASE 0xfe000000
