/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: init548x.c                                                |
+-----------------------------------------------------------------+
| The file contains the c part of MCF548x init code               |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

#include <rtems.h>
#include <bsp.h>

#if defined(HAS_LOW_LEVEL_INIT)
#define SYSTEM_PERIOD			10  	/* system bus period in ns */

/* SDRAM Timing Parameters */
#define SDRAM_TWR		    	2  		/* in clocks */
#define SDRAM_CASL		    	2.5 	/* in clocks */
#define SDRAM_TRCD		    	20		/* in ns */
#define SDRAM_TRP		    	20		/* in ns */
#define SDRAM_TRFC		    	75		/* in ns */
#define SDRAM_TREFI		    	7800	/* in ns */
#endif /* defined(HAS_LOW_LEVEL_INIT) */

extern uint8_t _DataRom[];
extern uint8_t _DataRam[];
extern uint8_t _DataEnd[];
extern uint8_t _BssStart[];
extern uint8_t _BssEnd[];
extern uint8_t _BootFlashBase[];
extern uint8_t _CodeFlashBase[];
extern uint8_t RamBase[];
extern uint32_t InterruptVectorTable[];
extern uint32_t _VectorRam[];

void gpio_init(void);
void fbcs_init(void);
void sdramc_init(void);
void mcf548x_init(void);


void mcf548x_init(void)
    {
    uint32_t n;
    uint8_t *dp, *sp;

#if defined(HAS_LOW_LEVEL_INIT)
    /* set XLB arbiter timeouts */
    MCF548X_XLB_ADRTO = 0x00000100;
    MCF548X_XLB_DATTO = 0x00000100;
    MCF548X_XLB_BUSTO = 0x00000100;
#endif

    gpio_init();
#if defined(HAS_LOW_LEVEL_INIT)
    fbcs_init();
    sdramc_init();
#endif /* defined(HAS_LOW_LEVEL_INIT) */

    /* Copy the vector table to RAM */
    if (_VectorRam != InterruptVectorTable)
    {
	for( n = 0; n < 256; n++)
	  {
      _VectorRam[n] = InterruptVectorTable[n];
	  }
    }

    m68k_set_vbr((uint32_t)_VectorRam);

    /* Move initialized data from ROM to RAM. */
    if (_DataRom != _DataRam)
    {
		n = _DataEnd - _DataRam;
        sp = (uint8_t *)_DataRom;
        dp = (uint8_t *)_DataRam;
		while(n--)
           *dp++ = *sp++;
     }

    /* Zero uninitialized data */
    if (_BssStart != _BssEnd)
    {
		n = _BssEnd - _BssStart;
        sp = (uint8_t *)_BssStart;
        while (n--)
            *sp++ = 0;
    }

}
/********************************************************************/
#if defined(HAS_LOW_LEVEL_INIT)
void
fbcs_init (void)
{
#ifdef M5484FIREENGINE

volatile uint32_t cscr, clk_ratio, fb_period, ws;

/* boot flash already valid ? */
if(!(MCF548X_FBCS_CSMR0 & MCF548X_FBCS_CSMR_V))
  {

    /*
     * Boot Flash
     */
    MCF548X_FBCS_CSAR0 = MCF548X_FBCS_CSAR_BA((uint32_t)(_BootFlashBase));

    cscr = (0
            | MCF548X_FBCS_CSCR_ASET(1)
            | MCF548X_FBCS_CSCR_WRAH(0)
            | MCF548X_FBCS_CSCR_RDAH(0)
            | MCF548X_FBCS_CSCR_AA
            | MCF548X_FBCS_CSCR_PS_16);

    /*
     * Determine the necessary wait states based on the defined system
     * period (XLB clock period) and the CLKIN to XLB ratio.
     * The boot flash has a max access time of 110ns.
     */
    clk_ratio = (MCF548X_PCI_PCIGSCR >> 24) & 0x7;
    fb_period = SYSTEM_PERIOD * clk_ratio;
    ws = 110 / fb_period;

    MCF548X_FBCS_CSCR0 = cscr | MCF548X_FBCS_CSCR_WS(ws);
    MCF548X_FBCS_CSMR0 = (0
                          | MCF548X_FBCS_CSMR_BAM_2M
                          | MCF548X_FBCS_CSMR_V);

  }

/* code flash already valid ? */
if(!(MCF548X_FBCS_CSMR1 & MCF548X_FBCS_CSMR_V))
  {

    /*
     * Code Flash
     */
    MCF548X_FBCS_CSAR1 = MCF548X_FBCS_CSAR_BA((uint32_t)(_CodeFlashBase));

    /*
     * Determine the necessary wait states based on the defined system
     * period (XLB clock period) and the CLKIN to XLB ratio.
     * The user/code flash has a max access time of 120ns.
     */
    ws = 120 / fb_period;
    MCF548X_FBCS_CSCR1 = cscr | MCF548X_FBCS_CSCR_WS(ws);
    MCF548X_FBCS_CSMR1 = (0
                          | MCF548X_FBCS_CSMR_BAM_16M
                          | MCF548X_FBCS_CSMR_V);
    }

#endif
}
#endif /* defined(HAS_LOW_LEVEL_INIT) */

/********************************************************************/
#if defined(HAS_LOW_LEVEL_INIT)
void
sdramc_init (void)
{

	/*
	 * Check to see if the SDRAM has already been initialized
	 * by a run control tool
	 */
	if (!(MCF548X_SDRAMC_SDCR & MCF548X_SDRAMC_SDCR_REF))
	{
		/*
         * Basic configuration and initialization
         */
		MCF548X_SDRAMC_SDRAMDS = (0
			| MCF548X_SDRAMC_SDRAMDS_SB_E(MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA)
			| MCF548X_SDRAMC_SDRAMDS_SB_C(MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA)
			| MCF548X_SDRAMC_SDRAMDS_SB_A(MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA)
			| MCF548X_SDRAMC_SDRAMDS_SB_S(MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA)
			| MCF548X_SDRAMC_SDRAMDS_SB_D(MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA)
            );
        MCF548X_SDRAMC_CS0CFG = (0
            | MCF548X_SDRAMC_CSnCFG_CSBA((uint32_t)(RamBase))
            | MCF548X_SDRAMC_CSnCFG_CSSZ(MCF548X_SDRAMC_CSnCFG_CSSZ_64MBYTE)
            );
        MCF548X_SDRAMC_SDCFG1 = (0
			| MCF548X_SDRAMC_SDCFG1_SRD2RW(7)
			| MCF548X_SDRAMC_SDCFG1_SWT2RD(SDRAM_TWR + 1)
			| MCF548X_SDRAMC_SDCFG1_RDLAT((int)((SDRAM_CASL*2) + 2))
			| MCF548X_SDRAMC_SDCFG1_ACT2RW((int)(((SDRAM_TRCD/SYSTEM_PERIOD) - 1) + 0.5))
			| MCF548X_SDRAMC_SDCFG1_PRE2ACT((int)(((SDRAM_TRP/SYSTEM_PERIOD) - 1) + 0.5))
			| MCF548X_SDRAMC_SDCFG1_REF2ACT((int)(((SDRAM_TRFC/SYSTEM_PERIOD) - 1) + 0.5))
			| MCF548X_SDRAMC_SDCFG1_WTLAT(3)
            );
		MCF548X_SDRAMC_SDCFG2 = (0
			| MCF548X_SDRAMC_SDCFG2_BRD2PRE(4)
			| MCF548X_SDRAMC_SDCFG2_BWT2RW(6)
			| MCF548X_SDRAMC_SDCFG2_BRD2WT(7)
			| MCF548X_SDRAMC_SDCFG2_BL(7)
            );

		/*
         * Precharge and enable write to SDMR
         */
        MCF548X_SDRAMC_SDCR = (0
			| MCF548X_SDRAMC_SDCR_MODE_EN
			| MCF548X_SDRAMC_SDCR_CKE
			| MCF548X_SDRAMC_SDCR_DDR
			| MCF548X_SDRAMC_SDCR_MUX(1)
			| MCF548X_SDRAMC_SDCR_RCNT((int)(((SDRAM_TREFI/(SYSTEM_PERIOD*64)) - 1) + 0.5))
			| MCF548X_SDRAMC_SDCR_IPALL
            );

		/*
         * Write extended mode register
         */
		MCF548X_SDRAMC_SDMR = (0
			| MCF548X_SDRAMC_SDMR_BNKAD_LEMR
			| MCF548X_SDRAMC_SDMR_AD(0x0)
			| MCF548X_SDRAMC_SDMR_CMD
            );

		/*
         * Write mode register and reset DLL
         */
		MCF548X_SDRAMC_SDMR = (0
			| MCF548X_SDRAMC_SDMR_BNKAD_LMR
			| MCF548X_SDRAMC_SDMR_AD(0x163)
			| MCF548X_SDRAMC_SDMR_CMD
            );

		/*
         * Execute a PALL command
         */
		MCF548X_SDRAMC_SDCR |=MCF548X_SDRAMC_SDCR_IPALL;

		/*
         * Perform two REF cycles
         */
		MCF548X_SDRAMC_SDCR |= MCF548X_SDRAMC_SDCR_IREF;
		MCF548X_SDRAMC_SDCR |= MCF548X_SDRAMC_SDCR_IREF;

		/*
         * Write mode register and clear reset DLL
         */
		MCF548X_SDRAMC_SDMR = (0
			| MCF548X_SDRAMC_SDMR_BNKAD_LMR
			| MCF548X_SDRAMC_SDMR_AD(0x063)
			| MCF548X_SDRAMC_SDMR_CMD
            );

		/*
         * Enable auto refresh and lock SDMR
         */
		MCF548X_SDRAMC_SDCR &= ~MCF548X_SDRAMC_SDCR_MODE_EN;
        MCF548X_SDRAMC_SDCR |= (0
			| MCF548X_SDRAMC_SDCR_REF
			| MCF548X_SDRAMC_SDCR_DQS_OE(0xF)
            );
    }

}
#endif /* defined(HAS_LOW_LEVEL_INIT) */

/********************************************************************/
void
gpio_init(void)
{

#ifdef M5484FIREENGINE

    /*
     * Enable Ethernet signals so that, if a cable is plugged into
     * the ports, the lines won't be floating and potentially cause
     * erroneous transmissions
     */
    MCF548X_GPIO_PAR_FECI2CIRQ = (0

        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_EMDC
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_EMDIO
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MII
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E17

        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MDC
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MDIO
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MII
        | MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E07
        );

#endif
    /* 
     * make sure the "edge port" has all interrupts disabled
     */
    MCF548X_EPORT_EPIER = 0;
}
