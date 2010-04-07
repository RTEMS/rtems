/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP startup code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>
#include <mpc55xx/emios.h>
#include <mpc55xx/siu.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define DEBUG_DONE() RTEMS_DEBUG_PRINT( "Done\n")

#define MPC55XX_INTERRUPT_STACK_SIZE 0x1000

/* Symbols defined in linker command file */
LINKER_SYMBOL(bsp_ram_start);
LINKER_SYMBOL(bsp_ram_end);
LINKER_SYMBOL(bsp_external_ram_start);
LINKER_SYMBOL(bsp_external_ram_size);
LINKER_SYMBOL(bsp_section_bss_end);

unsigned int bsp_clock_speed = 0;

uint32_t bsp_clicks_per_usec = 0;

void BSP_panic( char *s)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);

	printk( "%s PANIC %s\n", _RTEMS_version, s);

	while (1) {
		/* Do nothing */
	}
}

void _BSP_Fatal_error( unsigned n)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);

	printk( "%s PANIC ERROR %u\n", _RTEMS_version, n);

	while (1) {
		/* Do nothing */
	}
}

void bsp_predriver_hook()
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	RTEMS_DEBUG_PRINT( "Initialize eDMA ...\n");
	sc = mpc55xx_edma_init();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize eDMA");
	} else {
		DEBUG_DONE();
	}
}

#if ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))
/*
 * define init values for FMPLL ESYNCRx
 * (used in start.S/fmpll.S)
 */
#define EPREDIV_VAL (MPC55XX_FMPLL_PREDIV-1)
#define EMFD_VAL    (MPC55XX_FMPLL_MFD-16)
#define VCO_CLK_REF (MPC55XX_FMPLL_REF_CLOCK/(EPREDIV_VAL+1))
#define VCO_CLK_OUT (VCO_CLK_REF*(EMFD_VAL+16))
#define ERFD_VAL    ((VCO_CLK_OUT/MPC55XX_FMPLL_CLK_OUT)-1)

const struct fmpll_syncr_vals_t {
  union ESYNCR2_tag esyncr2_temp;
  union ESYNCR2_tag esyncr2_final;
  union ESYNCR1_tag esyncr1_final;
} fmpll_syncr_vals = 
  {
    { /* esyncr2_temp */
      .B.LOCEN=0,
      .B.LOLRE=0,
      .B.LOCRE=0,
      .B.LOLIRQ=0,
      .B.LOCIRQ=0,
      .B.ERATE=0,
      .B.DEPTH=0,
      .B.ERFD=ERFD_VAL+2 /* reduce output clock during init */
    },
    { /* esyncr2_final */
      .B.LOCEN=0,
      .B.LOLRE=0,
      .B.LOCRE=0,
      .B.LOLIRQ=0,
      .B.LOCIRQ=0,
      .B.ERATE=0,
      .B.DEPTH=0,
      .B.ERFD=ERFD_VAL /* nominal output clock after init */
    },
    { /* esyncr1_final */
      .B.CLKCFG=7,
      .B.EPREDIV=EPREDIV_VAL,
      .B.EMFD=EMFD_VAL
    }
  };

#else /* ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517)) */

const struct fmpll_syncr_vals_t {
  union SYNCR_tag syncr_temp;
  union SYNCR_tag syncr_final;
} fmpll_syncr_vals = 
  {
    { /* syncr_temp */
      .B.PREDIV=MPC55XX_FMPLL_PREDIV-1,
      .B.MFD=MPC55XX_FMPLL_MFD,
      .B.RFD=2,
      .B.LOCEN=1
    },
    { /* syncr_final */
      .B.PREDIV=MPC55XX_FMPLL_PREDIV-1,
      .B.MFD=MPC55XX_FMPLL_MFD,
      .B.RFD=0,
      .B.LOCEN=1
    }
  };

#endif /* ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517)) */

#if defined(BOARD_GWLCFM)
static const mpc55xx_siu_pcr_entry_t siu_pcr_list[] = {
  {  0,16,{.B.PA = 1,           .B.WPE = 0}}, /* PA[ 0..15] analog input */
  { 16, 4,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PB[ 0.. 4] LED/CAN_STBN out */
  { 20, 2,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PB[ 5.. 6] CAN_ERR/USBFLGC in*/
  { 22, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PB[ 7    ] FR_A_EN out */
  { 23, 4,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PB[ 8..10] IRQ/FR_A_ERR/USB_RDYin */
  { 27, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PB[11..11] FR_STBN out */

  { 32, 2,{.B.PA = 2,.B.OBE = 1,.B.WPE = 0}}, /* PC[ 0.. 1] FR_A_TX/TXEN out */
  { 34, 1,{.B.PA = 2,.B.IBE = 1,.B.WPE = 0}}, /* PC[ 2.. 2] FR_A_RX in */
  { 35, 2,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PC[ 3.. 4] INIT_ERR/ISB_IRQ in */
  { 37, 2,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PC[ 5.. 6] PWRO1/2_ON out */
  { 39, 1,{.B.PA = 2,.B.IBE = 1,.B.WPE = 0}}, /* PC[ 7.. 7] FR_B_RX in */
  { 40, 2,{.B.PA = 2,.B.OBE = 1,.B.WPE = 0}}, /* PC[ 8.. 9] FR_B_TX/TXEN out */
  { 42, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PC[10    ] FR_B_EN out */
  { 43, 1,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PC[11    ] FOR_STATUS in */
  { 44, 1,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PC[12    ] FR_B_ERRN  in */
  { 45, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PC[13    ] HS_CAN_STBN out */
  { 46, 1,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PC[14    ] HS_CAN_ERR in */
  { 47, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PC[15    ] HS_CAN_EN out */

  { 48, 1,{.B.PA = 1,.B.OBE = 1,.B.WPE = 0}}, /* PD[ 0    ] HS_CAN_TX out */
  { 49, 1,{.B.PA = 1,.B.IBE = 1,.B.WPE = 0}}, /* PD[ 1    ] HS_CAN_RX in  */
  { 50, 2,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PD[ 2.. 3] PWRO1/2_OC in */
  { 52, 1,{.B.PA = 1,.B.OBE = 1,.B.WPE = 0}}, /* PD[ 4    ] LS_CAN_TX out */
  { 53, 1,{.B.PA = 1,.B.IBE = 1,.B.WPE = 0}}, /* PD[ 5    ] LS_CAN_RX in  */
  { 54, 1,{.B.PA = 1,.B.OBE = 1,.B.WPE = 0}}, /* PD[ 6    ] HS_CAN_TX out */
  { 55, 1,{.B.PA = 1,.B.IBE = 1,.B.WPE = 0}}, /* PD[ 7    ] HS_CAN_RX in  */
  { 56, 1,{.B.PA = 2,.B.IBE = 1,.B.OBE = 1,.B.WPE = 0}}, 
  /* PD[ 8    ] I2C_SCL in/out */
  { 57, 1,{.B.PA = 2,.B.IBE = 1,.B.OBE = 1,.B.WPE = 0}}, 
  /* PD[ 9    ] I2C_SDA in/out */
  { 58, 4,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PD[10..13] LS_CAN_EN/LED out*/
  { 62, 4,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PD[14..15] USB_FLGA/B    in */

  { 64, 3,{.B.PA = 3,.B.SRC = 1,.B.WPE = 0}}, /* PE[ 0.. 2] MLBCLK/SI/DI  in */
  { 67, 2,{.B.PA = 3,.B.SRC = 1,.B.WPE = 0}}, /* PE[ 3.. 4] MLBSO/DO      out*/
  { 69, 1,{.B.PA = 3,.B.SRC = 1,.B.WPE = 0}}, /* PE[ 5.. 5] MLBSLOT       in */
  { 70, 1,{.B.PA = 1,.B.SRC = 3,.B.WPE = 0}}, /* PE[ 6.. 6] CLKOUT        out*/

  { 80, 1,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 0.. 0] RD_WR         out*/
  { 81, 1,{.B.PA = 0,.B.SRC = 0,.B.WPE = 0}}, /* PF[ 1.. 1] (nc)          in */
  { 82, 8,{.B.PA = 2,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 2..11] ADDR[8..15]   out*/
  { 90, 2,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 2..11] CS[0..1]      out*/
  { 92, 1,{.B.PA = 3,.B.SRC = 3,.B.WPE = 0}}, /* PF[    12] ALE           out*/
  { 93, 3,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[13..15] OE/WE         out*/

  { 96,16,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PG[ 0..15] AD16..31   in/out*/

  {112, 3,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 0.. 2] LED_EXT1-3.   out*/
  {115, 1,{.B.PA = 3,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 3.. 3] CS2_ETH       out*/
  {116, 2,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 4.. 5] FR/HC_TERM    out*/
  {118, 1,{.B.PA = 2,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 6.. 6] LIN_Tx        out*/
  {119, 1,{.B.PA = 2,.B.IBE = 1,.B.WPE = 0}}, /* PH[ 7.. 7] LIN_Rx        in */
  {120, 4,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 8..11] LIN_SLP,RST   out*/

  {0,0}
};

#else /* MPC55xxEVB */

static const mpc55xx_siu_pcr_entry_t siu_pcr_list[] = {
  {  0, 1,{.B.PA = 1,.B.DSC = 1,.B.WPE=1,.B.WPS=1}}, /* !CS  [0]      */
  {  3, 1,{.B.PA = 1,.B.DSC = 1,.B.WPE=1,.B.WPS=1}}, /* !CS  [3]      */
  {  4,24,{.B.PA = 1,.B.DSC = 1                  }}, /* ADDR [8 : 31] */
  { 28,16,{.B.PA = 1,.B.DSC = 1                  }}, /* DATA [0 : 15] */
  { 62, 8,{.B.PA = 1,.B.DSC = 1,.B.WPE=1,.B.WPS=1}}, /* RD_!WR, BDIP, 
							!WE, !OE, !TS */
  { 89, 2,{.B.PA = 1                             }}, /* ESCI_B        */

  {0,0}
};
#endif /* BOARD_GWLCFM */

static void mpc55xx_ebi_init(void)
{
	struct EBI_CS_tag cs = { .BR = MPC55XX_ZERO_FLAGS, .OR = MPC55XX_ZERO_FLAGS };
	struct MMU_tag mmu = MMU_DEFAULT;
	
	/*
	 * init I/O pins to proper state
	 */
	mpc55xx_siu_pcr_init(&SIU,
			     siu_pcr_list);

#if defined(BOARD_GWLCFM)
	/*
	 * init EBI for Muxed AD bus
	 */
	EBI.MCR.B.DBM = 1;
	EBI.MCR.B.ADMUX = 1; /* use multiplexed bus */
	EBI.MCR.B.D16_32 = 1; /* use lower AD bus    */

	SIU.ECCR.B.EBDF = 3;  /* use CLK/4 as bus clock */

	/* External SRAM (16 bit, 2 wait states, 512kB, no burst) */

	cs.BR.B.BA = 0;
	cs.BR.B.PS = 1;
	cs.BR.B.AD_MUX = 1;
	cs.BR.B.WEBS = 1;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1; 
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1fff0;
	cs.OR.B.SCY = 0;
	cs.OR.B.BSCY = 0;

	EBI.CS [0] = cs;

	/* External Ethernet Controller (3 wait states, 64kB) */

	mmu.MAS0.B.ESEL = 5;
	mmu.MAS1.B.VALID = 1;
	mmu.MAS1.B.IPROT = 1;
	mmu.MAS1.B.TSIZ = 1;
	mmu.MAS2.B.EPN = 0x3fff8;
	mmu.MAS2.B.I = 1;
	mmu.MAS2.B.G = 1;
	mmu.MAS3.B.RPN = 0x3fff8;
	mmu.MAS3.B.UW = 1;
	mmu.MAS3.B.SW = 1;
	mmu.MAS3.B.UR = 1;
	mmu.MAS3.B.SR = 1;

	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS0, mmu.MAS0.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS1, mmu.MAS1.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS2, mmu.MAS2.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS3, mmu.MAS3.R);

	asm volatile ("tlbwe");

	cs.BR.B.BA = 0x7fff;
	cs.BR.B.PS = 1;
	cs.BR.B.BL = 0;
	cs.BR.B.AD_MUX = 1;
	cs.BR.B.WEBS = 0;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1;
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1ffff;
	cs.OR.B.SCY = 1;
	cs.OR.B.BSCY = 0;

	EBI.CS [3] = cs;
#else /* defined(BOARD_GWLCFM) */
	
	/* External SRAM (2 wait states, 512kB, 4 word burst) */

	cs.BR.B.BA = 0;
	cs.BR.B.PS = 1;
	cs.BR.B.BL = 1;
	cs.BR.B.WEBS = 0;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1; /* TODO: Enable burst */
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1fff0;
	cs.OR.B.SCY = 0;
	cs.OR.B.BSCY = 0;

	EBI.CS [0] = cs;

	/* External Ethernet Controller (3 wait states, 64kB) */

	mmu.MAS0.B.ESEL = 5;
	mmu.MAS1.B.VALID = 1;
	mmu.MAS1.B.IPROT = 1;
	mmu.MAS1.B.TSIZ = 1;
	mmu.MAS2.B.EPN = 0x3fff8;
	mmu.MAS2.B.I = 1;
	mmu.MAS2.B.G = 1;
	mmu.MAS3.B.RPN = 0x3fff8;
	mmu.MAS3.B.UW = 1;
	mmu.MAS3.B.SW = 1;
	mmu.MAS3.B.UR = 1;
	mmu.MAS3.B.SR = 1;

	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS0, mmu.MAS0.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS1, mmu.MAS1.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS2, mmu.MAS2.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS3, mmu.MAS3.R);

	asm volatile ("tlbwe");

	cs.BR.B.BA = 0x7fff;
	cs.BR.B.PS = 1;
	cs.BR.B.BL = 0;
	cs.BR.B.WEBS = 0;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1;
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1ffff;
	cs.OR.B.SCY = 1;
	cs.OR.B.BSCY = 0;

	EBI.CS [3] = cs;
#endif /* defined(BOARD_GWLCFM) */
}

/**
 * @brief Start BSP.
 */
LINKER_SYMBOL(bsp_section_bss_start);
LINKER_SYMBOL(bsp_section_bss_end);
LINKER_SYMBOL(bsp_section_sbss_start);
LINKER_SYMBOL(bsp_section_sbss_end);
LINKER_SYMBOL(bsp_section_vector_start);

void bsp_start(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ppc_cpu_id_t myCpu;
	ppc_cpu_revision_t myCpuRevision;

	uintptr_t interrupt_stack_start = (uintptr_t)bsp_ram_end - 2 * MPC55XX_INTERRUPT_STACK_SIZE;
	uint32_t interrupt_stack_size = MPC55XX_INTERRUPT_STACK_SIZE;


	/* Initialize External Bus Interface */
	mpc55xx_ebi_init();

	/*
	 * make sure BSS/SBSS is cleared
	 */
	memset(bsp_section_bss_start,0,
	       bsp_section_bss_end-bsp_section_bss_start);
	memset(bsp_section_sbss_start,0,
	       bsp_section_sbss_end-bsp_section_sbss_start);

	ppc_exc_vector_base = bsp_section_vector_start;

	RTEMS_DEBUG_PRINT( "BSP start ...\n");

	RTEMS_DEBUG_PRINT( "System clock          : %i\n", mpc55xx_get_system_clock());
	RTEMS_DEBUG_PRINT( "Memory start          : 0x%08x\n", bsp_ram_start);
	RTEMS_DEBUG_PRINT( "Memory end            : 0x%08x\n", bsp_ram_end);
	RTEMS_DEBUG_PRINT( "Memory size           : 0x%08x\n", bsp_ram_end - bsp_ram_start);
	RTEMS_DEBUG_PRINT( "Interrupt stack start : 0x%08x\n", interrupt_stack_start);
	RTEMS_DEBUG_PRINT( "Interrupt stack end   : 0x%08x\n", interrupt_stack_start + interrupt_stack_size);
	RTEMS_DEBUG_PRINT( "Interrupt stack size  : 0x%08x\n", interrupt_stack_size);

	/*
	 * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
	 * function store the result in global variables so that it can be used
	 * latter...
	 */
	myCpu = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();

	/* Time reference value */
	bsp_clicks_per_usec = bsp_clock_speed / 1000000;

	/*
	 * determine clock speed
	 */
	bsp_clock_speed = mpc55xx_get_system_clock();

	/* Initialize exceptions */
	RTEMS_DEBUG_PRINT( "Initialize exceptions ...\n");
	sc = ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		interrupt_stack_start,
		interrupt_stack_size
	);
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize exceptions");
	} else {
		DEBUG_DONE();
	}

	/* Initialize interrupts */
	RTEMS_DEBUG_PRINT( "Initialize interrupts ...\n");
	sc = bsp_interrupt_initialize();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize interrupts");
	} else {
		DEBUG_DONE();
	}

	/* Initialize eMIOS */
	mpc55xx_emios_initialize( 1);

	mpc55xx_emios_set_global_prescaler(MPC55XX_EMIOS_PRESCALER);

	/*
	* Enable instruction and data caches. Do not force writethrough mode.
	*/
#if INSTRUCTION_CACHE_ENABLE
	rtems_cache_enable_instruction();
#endif
#if DATA_CACHE_ENABLE
	rtems_cache_enable_data();
#endif
}
