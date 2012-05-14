/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  Modified for mvme3100 by T. Straumann
 */

#include <string.h>
#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/spr.h>
#include <libcpu/io.h>
#include <libcpu/e500_mmu.h>
#include <bsp/uart.h>
#include <bsp/irq.h>
#include <bsp/pci.h>
#include <bsp/vpd.h>
#include <libcpu/cpuIdent.h>
#include <bsp/vectors.h>
#include <rtems/powerpc/powerpc.h>

#define SHOW_MORE_INIT_SETTINGS
#undef  DEBUG

#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef  DEBUG
#define STATIC
#else
#define STATIC static
#endif

extern unsigned long __rtems_end[];
extern void	         BSP_vme_config(void);
extern void          BSP_pciConfigDump_early( void );
extern unsigned      ppc_exc_lock_std, ppc_exc_gpr3_std;

/*
 * Copy Additional boot param passed by boot loader
 */
#define CMDLINE_BUF_SIZE	2048

static char cmdline_buf[CMDLINE_BUF_SIZE] = {0};
char *BSP_commandline_string         = cmdline_buf;

/*
 * Vital Board data Start using DATA RESIDUAL
 */
uint32_t bsp_clicks_per_usec         = 0;
/*
 * Total memory using RESIDUAL DATA
 */
unsigned int BSP_mem_size            = 0;
/*
 * PCI Bus Frequency
 */
unsigned int BSP_pci_bus_frequency   = 0xdeadbeef;
/*
 * PPC Bus Frequency
 */
unsigned int BSP_bus_frequency       = 0;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency = 0;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor   = 8000; /* if external RTC clock unused (HID0) */

/* Board identification string */
char BSP_productIdent[20]            = {0};
char BSP_serialNumber[20]            = {0};

/* VPD appends an extra char -- what for ? */
char BSP_enetAddr0[7]                = {0};
char BSP_enetAddr1[7]                = {0};
char BSP_enetAddr2[7]                = {0};

static void
prether(char *b, int idx)
{
int i;
	printk("Ethernet %i                  %02X", idx, *b++);
	for ( i=0; i<5; i++ )
		printk(":%02X",*b++);
	printk("\n");
}


BSP_output_char_function_type     BSP_output_char = BSP_output_char_via_serial;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

void BSP_panic(char *s)
{
  printk("\n%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc");
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("\n%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc");
}

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */

char * save_boot_params(void* r3, void *r4, void* r5, char *additional_boot_options)
{

  strncpy(cmdline_buf, additional_boot_options, CMDLINE_BUF_SIZE);
  cmdline_buf[CMDLINE_BUF_SIZE - 1] ='\0';
  return cmdline_buf;
}

#define CS_CONFIG_CS_EN (1<<31)
#define CS_BNDS_SA(x)	((((uint32_t)(x))>>(31-15)) & 0xff)
#define CS_BNDS_EA(x)	((((uint32_t)(x))>>(31-31)) & 0xff)

static inline uint32_t
_ccsr_rd32(uint32_t off)
{
	return in_be32( (volatile unsigned *)(BSP_8540_CCSR_BASE + off) );
}

static inline void
_ccsr_wr32(uint32_t off, uint32_t val)
{
	out_be32( (volatile unsigned *)(BSP_8540_CCSR_BASE + off), val );
}


STATIC uint32_t
BSP_get_mem_size( void )
{
int i;
uint32_t	cs_bnds, cs_config;
uint32_t	memsz=0;
uint32_t	v;

	for ( cs_bnds = 0x2000, cs_config=0x2080, i=0; i<4; i++, cs_bnds+=8, cs_config+=4 ) {
		if ( CS_CONFIG_CS_EN & _ccsr_rd32( cs_config ) ) {
			v = _ccsr_rd32( cs_bnds );

			memsz += CS_BNDS_EA(v) - CS_BNDS_SA(v) + 1;
		}
	}
	return memsz << 24;
}

STATIC void
BSP_calc_freqs( void )
{
uint32_t	porpllsr   = _ccsr_rd32( 0xe0000 );
unsigned	plat_ratio = (porpllsr >> (31-30)) & 0x1f;
unsigned    e500_ratio = (porpllsr >> (31-15)) & 0x3f;

	switch ( plat_ratio ) {
		case  2: case  3: case  4: case  5: case  6:
		case  8: case  9: case 10: case 12: case 16:
		/* supported ratios */
			BSP_bus_frequency = BSP_pci_bus_frequency * plat_ratio;
		break;

		default:
			BSP_panic("Unknown PLL sys-clock ratio; something's wrong here");
	}

	switch ( e500_ratio ) {
		case 4: case 5: case 6: case 7:
			BSP_processor_frequency = (BSP_pci_bus_frequency * e500_ratio) >> 1;
		break;

		default:
			BSP_panic("Unknown PLL e500-clock ratio; something's wrong here");
	}

	printk("Core Complex Bus (CCB) Clock Freq: %10u Hz\n", BSP_bus_frequency);
	printk("CPU Clock Freq:                    %10u Hz\n", BSP_processor_frequency);
}

void
bsp_predriver_hook(void)
{
	/* Some drivers (RTC) may need i2c */
	BSP_i2c_initialize();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

#include <libcpu/spr.h>

SPR_RW(HID1)

void bsp_start( void )
{
rtems_status_code   sc;
unsigned char       *stack;
uintptr_t           intrStackStart;
uintptr_t           intrStackSize;
char                *chpt;
ppc_cpu_id_t        myCpu;
ppc_cpu_revision_t  myCpuRevision;
int                 i;
E500_tlb_va_cache_t *tlb;

VpdBufRec          vpdData [] = {
	{ key: ProductIdent, instance: 0, buf: BSP_productIdent, buflen: sizeof(BSP_productIdent) - 1 },
	{ key: SerialNumber, instance: 0, buf: BSP_serialNumber, buflen: sizeof(BSP_serialNumber) - 1 },
	{ key: BusClockHz,   instance: 0, buf: &BSP_pci_bus_frequency, buflen: sizeof(BSP_pci_bus_frequency)  },
	{ key: EthernetAddr, instance: 0, buf: BSP_enetAddr0, buflen: sizeof(BSP_enetAddr0) },
	{ key: EthernetAddr, instance: 1, buf: BSP_enetAddr1, buflen: sizeof(BSP_enetAddr1) },
	{ key: EthernetAddr, instance: 2, buf: BSP_enetAddr2, buflen: sizeof(BSP_enetAddr2) },
	VPD_END
};

	/* Intersperse messages with actions to help locate problems */
	printk("-----------------------------------------\n");

	/*
	 * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
	 * function store the result in global variables so that it can be used
	 * later...
	 */
	myCpu 	      = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();

	printk("Welcome to %s\n", _RTEMS_version);
	printk("BSP: %s, CVS Release ($Name$)\n", "mvme3100");

	/*
	 * the initial stack  has aready been set to this value in start.S
	 * so there is no need to set it in r1 again... It is just for info
	 * so that It can be printed without accessing R1.
	 */
	asm volatile("mr %0, 1":"=r"(stack));

	/* tag the bottom */
	*((uint32_t*)stack) = 0;

	/*
	 * Initialize the interrupt related settings.
	 */
	intrStackStart = (uintptr_t) __rtems_end;
	intrStackSize = rtems_configuration_get_interrupt_stack_size();

	/*
	 * Initialize default raw exception handlers.
	 */
	sc = ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		intrStackStart,
		intrStackSize
	);
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic("cannot initialize exceptions");
	}

	printk("CPU 0x%x - rev 0x%x\n", myCpu, myCpuRevision);

#ifdef SHOW_MORE_INIT_SETTINGS
	printk("Additionnal boot options are %s\n", BSP_commandline_string);
	printk("Initial system stack at %x\n",      stack);
	printk("Software IRQ stack starts at %x with size %u\n", intrStackStart, intrStackSize);
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
	printk("Going to start PCI buses scanning and initialization\n");
#endif

	BSP_mem_size            = BSP_get_mem_size();

	{
		/* memory-select errors were disabled in 'start.S';
		 * motload has all TLBs mapping a possible larger area as
		 * memory (not-guarded, caching-enabled) than actual physical
		 * memory is available.
		 * In case of speculative loads this may cause 'memory-select' errors
		 * which seem to raise 'core_fault_in' (found no description in
		 * the manual but I experienced this problem).
		 * Such errors (if HID1[RFXE] is clear) may *stall* execution
		 * leading to mysterious 'hangs'.
		 *
		 * Here we remove all mappings, re-enable memory-select
		 * errors and make sure we enable HID1[RFXE] to avoid
		 * stalls (since we don't implement handling individual
		 * error-handling interrupts).
		 */

		/* enable machine check for bad bus errors */
		_write_HID1( _read_HID1() | 0x20000 );

		rtems_e500_initlb();

		for ( i=0, tlb=rtems_e500_tlb_va_cache; i<NumberOf(rtems_e500_tlb_va_cache); i++, tlb++ ) {
			/* disable TLBs for caching-enabled, non-guarded areas
			 * beyond physical memory
			 */
			if (    tlb->att.v
			    &&  0xa != (tlb->att.wimge & 0xa)
				&&  (tlb->va.va_epn<<12) >= BSP_mem_size ) {
				rtems_e500_clrtlb( E500_SELTLB_1 | i );
			}
		}

		/* clear all pending memory errors */
		_ccsr_wr32(0x2e40, 0xffffffff);
		/* enable checking for memory-select errors */
		_ccsr_wr32(0x2e44, _ccsr_rd32(0x2e44) & ~1 );
	}

	BSP_vpdRetrieveFields( vpdData );

	printk("Board Type: %s (S/N %s)\n",
			BSP_productIdent[0] ? BSP_productIdent : "n/a",
			BSP_serialNumber[0] ? BSP_serialNumber : "n/a");

	printk("External (=PCI Bus) Clock Freq   ");
	if ( 0xdeadbeef == BSP_pci_bus_frequency ) {
		BSP_pci_bus_frequency	= 66666666;
		printk(" NOT FOUND in VPD; using %10u Hz\n",
				BSP_pci_bus_frequency);
	} else {
		printk(": %10u Hz\n",
				BSP_pci_bus_frequency);
	}

	/* Calculate CPU and CCB bus freqs */
	BSP_calc_freqs();

	pci_initialize();

	prether(BSP_enetAddr0, 0);
	prether(BSP_enetAddr1, 1);
	prether(BSP_enetAddr2, 2);

	/* need to tweak the motload setup */
	BSP_motload_pci_fixup();

#ifdef SHOW_MORE_INIT_SETTINGS
	printk("Number of PCI buses found is : %d\n", pci_bus_count());
	{
		BSP_pciConfigDump_early();
	}
#endif

	if ( (chpt = strstr(BSP_commandline_string,"MEMSZ=")) ) {
		char		*endp;
		uint32_t 	sz;
		chpt+=6 /* strlen("MEMSZ=") */;
		sz = strtoul(chpt, &endp, 0);
		if ( endp != chpt )
			BSP_mem_size = sz;
	}

	printk("Memory:                            %10u bytes\n", BSP_mem_size);

	BSP_bus_frequency       = 333333333;
	BSP_processor_frequency = 833333333;
	BSP_time_base_divisor   = 8000; /* if external RTC clock unused (HID0) */

	/* clear hostbridge errors but leave MCP disabled -
	 * PCI config space scanning code will trip otherwise :-(
	 */
	_BSP_clear_hostbridge_errors(0 /* enableMCP */, 0/*quiet*/);

	bsp_clicks_per_usec = BSP_bus_frequency/(BSP_time_base_divisor * 1000);

	/*
	 * Initalize RTEMS IRQ system
	 */
	BSP_rtems_irq_mng_init(0);

	if (1) {
		int i;
		unsigned msr,tcr;
		asm volatile("mfmsr %0":"=r"(msr));
		asm volatile("mftcr %0":"=r"(tcr));
		printk("MSR is 0x%08x, TCR 0x%08x\n",msr,tcr);
		asm volatile("mttcr %0"::"r"(0));
		if (0) {
			asm volatile("mtmsr %0"::"r"(msr|0x8000));
			for (i=0; i<12; i++)
				BSP_enable_irq_at_pic(i);
			printk("IRQS enabled\n");
		}
	}

	if (0) {
		unsigned x;
		asm volatile("mfivpr %0":"=r"(x));
		printk("IVPR: 0x%08x\n",x);
		asm volatile("mfivor8 %0":"=r"(x));
		printk("IVOR8: 0x%08x\n",x);
		printk("0x%08x\n",*(unsigned *)0xc00);
		printk("0x%08x\n",*(unsigned *)0xc04);
		printk("0x%08x\n",*(unsigned *)0xc08);
		printk("0x%08x\n\n\n",*(unsigned *)0xc0c);
		if (0) {
			*(unsigned *)0xc08 = 0x4c000064;
			asm volatile("dcbf 0, %0; icbi 0, %0; sync; isync"::"r"(0xc00));
		}

		printk("0x%08x\n", ppc_exc_lock_std);
		printk("0x%08x\n", ppc_exc_gpr3_std);

		asm volatile("sc");

		printk("0x%08x\n", ppc_exc_lock_std);
		printk("0x%08x\n", ppc_exc_gpr3_std);
	}

	printk("-----------------------------------------\n");

#ifdef SHOW_MORE_INIT_SETTINGS
	printk("Exit from bspstart\n");
#endif

}
