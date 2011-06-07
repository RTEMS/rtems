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
  
  { 58, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PD[10] LS_CAN_EN     out*/
  { 59, 3,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, 
  /* PD[11..13] PWO1_OC, MOCO_INT in */
  
  { 62, 4,{.B.PA = 0,.B.IBE = 1,.B.WPE = 0}}, /* PD[14..15] USB_FLGA/B    in */

  { 64, 5,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PE[ 0.. 4] LED_EXT1-5.   out*/
  { 70, 1,{.B.PA = 1,.B.SRC = 3,.B.WPE = 0}}, /* PE[ 6.. 6] CLKOUT        out*/

  { 80, 1,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 0.. 0] RD_WR         out*/
  { 81, 1,{.B.PA = 0,.B.SRC = 0,.B.WPE = 0}}, /* PF[ 1.. 1] (nc)          in */
  { 82, 8,{.B.PA = 2,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 2..11] ADDR[8..15]   out*/
  { 90, 2,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[ 2..11] CS[0..1]      out*/
  { 92, 1,{.B.PA = 3,.B.SRC = 3,.B.WPE = 0}}, /* PF[    12] ALE           out*/
  { 93, 3,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PF[13..15] OE/WE         out*/

  { 96,16,{.B.PA = 1,.B.SRC = 1,.B.WPE = 0}}, /* PG[ 0..15] AD16..31   in/out*/

  {113, 1,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 1.. 1] RES_MOSTComp  out*/
  {114, 1,{.B.PA = 3,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 2.. 2] CS3_MOSTComp  out*/
  {115, 1,{.B.PA = 3,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 3.. 3] CS2_ETH       out*/
  {116, 2,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 4.. 5] FR/HC_TERM    out*/
  {118, 1,{.B.PA = 2,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 6.. 6] LIN_Tx        out*/
  {119, 1,{.B.PA = 2,.B.IBE = 1,.B.WPE = 0}}, /* PH[ 7.. 7] LIN_Rx        in */
  {120, 4,{.B.PA = 0,.B.OBE = 1,.B.WPE = 0}}, /* PH[ 8..11] LIN_SLP,RST   out*/

  {0,0}
};
#elif defined(BOARD_PHYCORE_MPC5554)

static const mpc55xx_siu_pcr_entry_t siu_pcr_list[] = {
  {  0, 4,{.B.PA = 1,          .B.DSC = 1,.B.WPE=1,.B.WPS=1}}, /* !CS  [0:3]    */
  {  4,24,{.B.PA = 1,          .B.DSC = 1                  }}, /* ADDR [8 : 31] */
  { 28,32,{.B.PA = 1,          .B.DSC = 1                  }}, /* DATA [0 : 31] */
  { 60, 4,{.B.PA = 1,          .B.DSC = 1,                 }}, /* TSIZ[0:1], RD_!WR, BDIP */
  { 64, 6,{.B.PA = 1,          .B.DSC = 1,.B.WPE=1,.B.WPS=1}}, /* RD_!WR, BDIP, !WE, !OE, !TS */
  { 89, 4,{.B.PA = 1                                       }}, /* ESCI_A and ESCI_B        */
  {229, 4,{          .B.OBE= 1,.B.DSC = 1                  }}, /* CLKOUT */

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

/*
 * Arrays for setting up the chip selects.
 * You can define up to four, and those with the valid bit
 * set will be loaded into the matching chip select.
 */
static const struct EBI_CS_tag cs_setup[] = {
#if defined(BOARD_GWLCFM)
	/* CS0: External SRAM (16 bit, 1 wait states, 512kB, no burst) */
	{
        {
            .B.BA = 0x20000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 1,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1fff0,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    },
	/* CS1: External USB controller (16 bit, 3 wait states, 32kB, no burst) */
	{
        {
            .B.BA = 0x22000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1ffff,
            .B.SCY = 3,
            .B.BSCY = 0
        }
    },
	/* CS2: Ethernet (16 bit, 2 wait states, 32kB, no burst) */
	{
        {
            .B.BA = 0x22800000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 1,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1ffff,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    },
    {				/* CS3: MOST Companion. */
        {
            .B.BA = 0x23000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1, 
            .B.V = 1
        },

        {
            .B.AM = 0x1fff0,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    }
#elif defined(BOARD_PHYCORE_MPC5554)
    /* CS0: External flash. */
    {
        { .R = 0x20000003 },   /* Base 0x2000000, Burst Inhibit, Valid */
        { .R = 0xff000050 }
    },
    /* CS1: External synchronous burst mode SRAM. */
    {
        { .R = 0x21000051 },   /* Base 0x2100000, 4-word Burst Enabled, Valid */
        { .R = 0xff000000 }    /* No wait states. */
    },
    /* CS2: External LAN91C111 */
    {
        { .R = 0x22000003 },   /* Base 0x22000000, Burst inhibit, valid */
        { .R = 0xff000010 }
    },

    /* CS3: External FPGA */
    {
        { .R = 0x23000003 },   /* Base 0x23000000, Burst inhibit, valid. */
        { .R = 0xff000020 }
    }
#else /* default, MPC55xxEVB */
	/* CS0: External SRAM (2 wait states, 512kB, 4 word burst) */
    {
        {
            .B.BA = 0,
            .B.PS = 1,
            .B.BL = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1, /* TODO: Enable burst */
            .B.V = 1
        },

        {
            .B.AM = 0x1fff0,
            .B.SCY = 0,
            .B.BSCY = 0
        }
    }, 
    { { .R = 0 }, { .R = 0 } },   /* CS1: Unused. */
    { { .R = 0 }, { .R = 0 } },   /* CS2: Unused. */
    {   /* CS3: ethernet? */
        {
            .B.BA = 0x7fff,
            .B.PS = 1,
            .B.BL = 0,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },

        {
            .B.AM = 0x1ffff,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    }
#endif /* Chip select setup */
};

/*
 * Arrays for setting up the MAS registers.
 * You can set as many as you want,we determine the size using sizeof.
 */
static const struct MMU_tag mmu_setup[] = {
#if defined(BOARD_GWLCFM)
    {
        /* External Ethernet Controller (3 wait states, 64kB) */

        {
            .B.TLBSEL = 1,      /* MAS0 */
            .B.ESEL = 5
        },
        {
            .B.VALID = 1,       /* MAS1 */
            .B.IPROT = 1,
            .B.TSIZ = 1
        },
        {
            .B.EPN = 0x3fff8,   /* MAS2 */
            .B.I = 1,
            .B.G = 1
        },
        {
            .B.RPN = 0x3fff8,   /* MAS3 */
            .B.UW = 1,
            .B.SW = 1,
            .B.UR = 1,
            .B.SR = 1
        }
    }

#elif defined(BOARD_PHYCORE_MPC5554)

    /* XXX I'm not using TLB1 entry 2 the same way as 
	 * in the BAM.
     */
    /*  Set up MMU TLB1 entry 2 for external ram. */
    /*  Effective Base address = 0x2100_0000 XXX NOT LIKE BAM */
    /*       Real Base address = 0x2100_0000 XXX NOT LIKE BAM */
    /*  Page Size            6 =  4MB XXX Not like BAM */
    /*  Not Guarded, Cache Enable, All Access (0, 3F) */
    {
        { .R = 0x10020000},     /* MAS0 */
        { .R = 0xC0000600},     /* MAS1 */
        { .R = 0x21000000},     /* MAS2 */
        { .R = 0x2100003F}      /* MAS3 */
    },

    /*  Set up MMU TLB1 entry 5 for second half of SRAM (debug RAM) */
    /*  Effective Base address = 0x2140_0000 */
    /*       Real Base address = 0x2140_0000 */
    /*  Page Size            6 = 4MB */
    /*  Not Guarded, Cache Enable, All Access (0, 3F) */
    {
        { .R =  0x10050000 },   /* MAS0 */
        { .R =  0xC0000600 },   /* MAS1 */
        { .R =  0x21400000 },   /* MAS2 */
        { .R =  0x2140003F }    /* MAS3 */
    },
    /*  Set up MMU TLB1 entry 6 for External LAN91C111 */
    /*  Effective Base address = 0x2200_0000 */
    /*       Real Base address = 0x2200_0000 */
    /*  Page Size            7 = 16MB */
    /*  Write-through, Guarded, Cache Inhibit, All Access (E, 3F) */
    {
        { .R = 0x10060000},     /* MAS0 */
        { .R = 0xC0000700},     /* MAS1 */
        { .R = 0x2200000E},     /* MAS2 */
        { .R = 0x2200003F}      /* MAS3 */
    },

    /*  Set up MMU TLB1 entry 7 for External FPGA */
    /*  Effective Base address = 0x2300_0000 */
    /*       Real Base address = 0x2300_0000 */
    /*  Page Size            7 = 16MB */
    /*  Write-through, Guarded, Cache Inhibit, All Access (E, 3F) */
    {
        { .R = 0x10070000},     /* MAS0 */
        { .R = 0xC0000700},     /* MAS1 */
        { .R = 0x2300000E},     /* MAS2 */
        { .R = 0x2300003F},     /* MAS3 */
    },

	/* Should also set up maps for the debug RAM and the
	 * external flash.
	 */
#else /* default, MPC55xxEVB */
    {
        /* External Ethernet Controller (3 wait states, 64kB) */
        .MAS0 = { .R = 0x10050000 },
        .MAS1 = { .R = 0xc0000100 },
        .MAS2 = { .R = 0x3fff800a },
        .MAS3 = { .R = 0x3fff800f }
    }
#endif /* MMU setup */
};

#ifdef MPC55XX_BOOTFLAGS
/* mpc55xx_bootflag_0 is defined in start.S using PUBLIC_VAR().  I go through this
 * indirection to avoid a linker issue - if I try to reference
 * mpc55xx_bootflag_0 as an "extern uint32_t" I get a linker error.
 * Maybe if I declare it as an "extern const uint32_t"?  Anyway, this works.
 */
extern void *mpc55xx_bootflag_0(void);
uint32_t *p_mpc55xx_bootflag_0 = (uint32_t *)mpc55xx_bootflag_0;
#endif

static void mpc55xx_ebi_init(void)
{
    int i;
	
#if defined(BOARD_GWLCFM)
    SIU.GPDO[122].B.PDO=1; /* make sure USB reset is kept high */
    SIU.GPDO[121].B.PDO=1; /* make sure Ethernet reset is kept high */
    SIU.GPDO[113].B.PDO=1; /* make sure MOST Companion reset is kept high */
#endif /* defined(BOARD_GWLCFM) */
	/*
	 * init I/O pins to proper state
	 */
	mpc55xx_siu_pcr_init(&SIU,
			     siu_pcr_list);

    /* Set up chip selects. */
    for (i = 0; i < sizeof(cs_setup) / sizeof(cs_setup[0]); i++) {
        if (cs_setup[i].BR.B.V) {
            EBI.CS [i] = cs_setup[i];
        }
    }

#ifdef MPC55XX_BOOTFLAGS
    /* If the low bit of bootflag 0 is clear don't change the MMU.
     */
    if (((*p_mpc55xx_bootflag_0) & 1))
#endif
      {
        /* Set up MMU. */
        for (i = 0; i < sizeof(mmu_setup) / sizeof(mmu_setup[0]); i++) {
            PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, mmu_setup[i].MAS0.R);
            PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS1, mmu_setup[i].MAS1.R);
            PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS2, mmu_setup[i].MAS2.R);
            PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS3, mmu_setup[i].MAS3.R);
            __asm__ volatile ("tlbwe");
        }
      }

#if defined(BOARD_GWLCFM)
	/*
	 * init EBI for Muxed AD bus
	 */
	EBI.MCR.B.DBM = 1;
	EBI.MCR.B.ADMUX = 1; /* use multiplexed bus */
	EBI.MCR.B.D16_32 = 1; /* use lower AD bus    */

	SIU.ECCR.B.EBDF = 3;  /* use CLK/4 as bus clock */

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

	ppc_exc_vector_base = (uint32_t) bsp_section_vector_start;

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

	/*
	 * determine clock speed
	 */
	bsp_clock_speed = mpc55xx_get_system_clock();

	/* Time reference value */
	bsp_clicks_per_usec = bsp_clock_speed / 1000000;

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
	ppc_exc_set_handler(ASM_ALIGN_VECTOR, ppc_exc_alignment_handler);

	/* Initialize interrupts */
	RTEMS_DEBUG_PRINT( "Initialize interrupts ...\n");
	sc = bsp_interrupt_initialize();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize interrupts");
	} else {
		DEBUG_DONE();
	}

	/* Initialize eMIOS */
	mpc55xx_emios_initialize( MPC55XX_EMIOS_PRESCALER);
}
