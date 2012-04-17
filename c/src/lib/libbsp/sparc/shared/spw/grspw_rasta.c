/* Select PCI driver */
#define GRSPW_PCI

#undef GRSPW_MAXDEVS
#undef DEBUG_SPACEWIRE_ONOFF
/*#define DEBUG_SPACEWIRE_ONOFF*/
/*
 * If USE_AT697_RAM is defined the RAM on the AT697 board will be used for DMA buffers (but rx message queue is always in AT697 ram).
 * USE_AT697_DMA specifies whether the messages will be fetched using DMA or PIO.
 *
 * RASTA_PCI_BASE is the base address of the GRPCI AHB slave
 *
  */

#define USE_AT697_RAM              1
#define USE_AT697_DMA              0
#define RASTA_PCI_BASE             0xe0000000
#define GRSPW_RASTA_MEM_OFF        0x21000

/* Make GRSPW driver use malloced or static memory
 */
#ifdef USE_AT697_RAM
#undef GRSPW_STATIC_MEM
#else
#define GRSPW_STATIC_MEM
#define GRSPW_CALC_MEMOFS(maxcores,corenum,ptr_mem_base,ptr_mem_end,ptr_bdtable_base) \
	grspw_rasta_calc_memoffs((maxcores),(corenum),(ptr_mem_base),(ptr_mem_end),(ptr_bdtable_base))
#endif

/* We have custom address tranlation for HW addresses */
#define GRSPW_ADR_TO

/* MEMAREA=>CPU used when reading descriptor buffer pointers,
 * they need to be translated from adresses used by GRSPW HW
 * into CPU readable addresses.
 *
 * NOT NEEDED AS GRSPW DRIVER USES INDEXES TO GET DESCRIPTOR
 * DATA POINTER ADDRESSES.
 */
#undef GRSPW_ADR_FROM

/* Set registered device name */
#define GRSPW_DEVNAME "/dev/grspwrasta0"
#define GRSPW_DEVNAME_NO(devstr,no) ((devstr)[15]='0'+(no))

/* Any non-static function will begin with */
#define GRSPW_PREFIX(name) grspwrasta##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling grspw_interrupt_handler.
 */
#define GRSPW_REG_INT(handler,irq,arg) \
  if ( grspw_rasta_int_reg ) \
    grspw_rasta_int_reg(handler,irq,arg);

#define GRSPW_DONT_BYPASS_CACHE

#ifdef GRSPW_ADR_TO
/* Translate a address within the Memory Region (memarea) into an Hardware
 * device address. This address is put into hardware registers or descriptors
 * so that the hardware can access the Memory Region.
 * Example:
 * An local AMBA access at 0xe0000000 will translate into PCI address 0x40000000,
 * the PCI address 0x40000000 will translate into CPU-AMBA address 0x40000000.
 */
static inline unsigned int memarea_to_hw(unsigned int addr) {
    return ((addr & 0x0fffffff) | RASTA_PCI_BASE);
}
#endif

void (*grspw_rasta_int_reg)(void *handler, int irq, void *arg) = 0;

#ifdef GRSPW_STATIC_MEM
static int grspw_rasta_calc_memoffs(int maxcores, int corenum, unsigned int *mem_base, unsigned int *mem_end, unsigned int *bdtable_base);
#endif

int grspw_rasta_interrupt_handler(unsigned int status);

void grspwrasta_interrupt_handler(int irq, void *pDev);

#include "grspw.c"

unsigned int grspw_rasta_memarea_address;

/* Register RASTA GRSPW driver.
 *
 * memarea     = preallocated memory somewhere, pointer to start of memory.
 */

int grspw_rasta_register(
 struct ambapp_bus *bus,
 unsigned int ram_base
 )
{
	/* Setup configuration */

	/* if zero the malloc will be used */
	grspw_rasta_memarea_address = ram_base + GRSPW_RASTA_MEM_OFF;

	/* Register the driver */
	return GRSPW_PREFIX(_register)(bus);
}

#if 0
/* Call this from PCI interrupt handler, simply figures out
 * which GRSPW core was responsible for the IRQ (may be multiple).
 * v = status of the PCI/AMBA MCPU IRQ CTRL
 */
int grspw_rasta_interrupt_handler(unsigned int status)
{
        int minor;

        for(minor = 0; minor < spw_cores; minor++) {
                if (status & (1<<grspw_devs[minor].irq) ) {
                        grspw_interrupt(&grspw_devs[minor]);
                }
        }
}
#endif

void GRSPW_PREFIX(_interrupt_handler)(int irq, void *pDev)
{
  grspw_interrupt(pDev);
}


#ifdef GRSPW_STATIC_MEM
/*
 *  --------------------------------------- <-
 *  | Core1: BD TABLE 1 and 2             |
 *  | Core2: BD TABLE 1 and 2             |
 *  | Core3: BD TABLE 1 and 2             |
 *  |-------------------------------------|
 *  | Core1: rx data buf + rx header buf  |
 *  | Core2: rx data buf + rx header buf  |
 *  | Core3: rx data buf + rx header buf  |
 *  ---------------------------------------
 */
static int grspw_rasta_calc_memoffs(int maxcores, int corenum, unsigned int *mem_base, unsigned int *mem_end, unsigned int *bdtable_base)
{
	if ( maxcores > 3 )
		return -1;

	if ( bdtable_base )
		*bdtable_base = grspw_rasta_memarea_address + corenum*2*SPACEWIRE_BDTABLE_SIZE;

	if ( mem_base )
		*mem_base = grspw_rasta_memarea_address + coremax*2*SPACEWIRE_BDTABLE_SIZE + corenum*BUFMEM_PER_LINK;

	if ( mem_end )
		*mem_end = grspw_rasta_memarea_address + coremax*2*SPACEWIRE_BDTABLE_SIZE + (corenum+1)*BUFMEM_PER_LINK;

	return 0;
}
#endif
