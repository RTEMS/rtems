/*
 * MPC8xx Internal Memory Map
 * Copyright (c) 1997 Dan Malek (dmalek@jlc.net)
 *
 * The I/O on the MPC860 is comprised of blocks of special registers
 * and the dual port ram for the Communication Processor Module.
 * Within this space are functional units such as the SIU, memory
 * controller, system timers, and other control functions.  It is
 * a combination that I found difficult to separate into logical
 * functional files.....but anyone else is welcome to try.  -- Dan
 */
#ifndef __IMMAP_8XX__
#define __IMMAP_8XX__

/* System configuration registers.
*/
typedef	struct sys_conf {
	unsigned int	sc_siumcr;
	unsigned int	sc_sypcr;
	unsigned int	sc_swt;
	char		res1[2];
	unsigned short	sc_swsr;
	unsigned int	sc_sipend;
	unsigned int	sc_simask;
	unsigned int	sc_siel;
	unsigned int	sc_sivec;
	unsigned int	sc_tesr;
	char		res2[0xc];
	unsigned int	sc_sdcr;
	char		res3[0x4c];
} sysconf8xx_t;

/* PCMCIA configuration registers.
*/
typedef struct pcmcia_conf {
	unsigned int	pcmc_pbr0;
	unsigned int	pcmc_por0;
	unsigned int	pcmc_pbr1;
	unsigned int	pcmc_por1;
	unsigned int	pcmc_pbr2;
	unsigned int	pcmc_por2;
	unsigned int	pcmc_pbr3;
	unsigned int	pcmc_por3;
	unsigned int	pcmc_pbr4;
	unsigned int	pcmc_por4;
	unsigned int	pcmc_pbr5;
	unsigned int	pcmc_por5;
	unsigned int	pcmc_pbr6;
	unsigned int	pcmc_por6;
	unsigned int	pcmc_pbr7;
	unsigned int	pcmc_por7;
	char		res1[0x20];
	unsigned int	pcmc_pgcra;
	unsigned int	pcmc_pgcrb;
	unsigned int	pcmc_pscr;
	char		res2[4];
	unsigned int	pcmc_pipr;
	char		res3[4];
	unsigned int	pcmc_per;
	char		res4[4];
} pcmconf8xx_t;

/* Memory controller registers.
*/
typedef struct	mem_ctlr {
	unsigned int	memc_br0;
	unsigned int	memc_or0;
	unsigned int	memc_br1;
	unsigned int	memc_or1;
	unsigned int	memc_br2;
	unsigned int	memc_or2;
	unsigned int	memc_br3;
	unsigned int	memc_or3;
	unsigned int	memc_br4;
	unsigned int	memc_or4;
	unsigned int	memc_br5;
	unsigned int	memc_or5;
	unsigned int	memc_br6;
	unsigned int	memc_or6;
	unsigned int	memc_br7;
	unsigned int	memc_or7;
	char		res1[0x24];
	unsigned int	memc_mar;
	unsigned int	memc_mcr;
	char		res2[4];
	unsigned int	memc_mamr;
	unsigned int	memc_mbmr;
	unsigned short	memc_mstat;
	unsigned short	memc_mptpr;
	unsigned int	memc_mdr;
	char		res3[0x80];
} memctl8xx_t;

/* System Integration Timers.
*/
typedef struct	sys_int_timers {
	unsigned short	sit_tbscr;
	unsigned int	sit_tbreff0;
	unsigned int	sit_tbreff1;
	char		res1[0x14];
	unsigned short	sit_rtcsc;
	unsigned int	sit_rtc;
	unsigned int	sit_rtsec;
	unsigned int	sit_rtcal;
	char		res2[0x10];
	unsigned short	sit_piscr;
	char		res3[2];
	unsigned int	sit_pitc;
	unsigned int	sit_pitr;
	char		res4[0x34];
} sit8xx_t;

#define TBSCR_TBIRQ_MASK	((unsigned short)0xff00)
#define TBSCR_REFA		((unsigned short)0x0080)
#define TBSCR_REFB		((unsigned short)0x0040)
#define TBSCR_REFAE		((unsigned short)0x0008)
#define TBSCR_REFBE		((unsigned short)0x0004)
#define TBSCR_TBF		((unsigned short)0x0002)
#define TBSCR_TBE		((unsigned short)0x0001)

#define RTCSC_RTCIRQ_MASK	((unsigned short)0xff00)
#define RTCSC_SEC		((unsigned short)0x0080)
#define RTCSC_ALR		((unsigned short)0x0040)
#define RTCSC_38K		((unsigned short)0x0010)
#define RTCSC_SIE		((unsigned short)0x0008)
#define RTCSC_ALE		((unsigned short)0x0004)
#define RTCSC_RTF		((unsigned short)0x0002)
#define RTCSC_RTE		((unsigned short)0x0001)

#define PISCR_PIRQ_MASK		((unsigned short)0xff00)
#define PISCR_PS		((unsigned short)0x0080)
#define PISCR_PIE		((unsigned short)0x0004)
#define PISCR_PTF		((unsigned short)0x0002)
#define PISCR_PTE		((unsigned short)0x0001)

/* Clocks and Reset.
*/
typedef struct clk_and_reset {
	unsigned int	car_sccr;
	unsigned int	car_plprcr;
	unsigned int	car_rsr;
	char		res[0x74];        /* Reserved area                  */
} car8xx_t;

/* System Integration Timers keys.
*/
typedef struct sitk {
	unsigned int	sitk_tbscrk;
	unsigned int	sitk_tbreff0k;
	unsigned int	sitk_tbreff1k;
	unsigned int	sitk_tbk;
	char		res1[0x10];
	unsigned int	sitk_rtcsck;
	unsigned int	sitk_rtck;
	unsigned int	sitk_rtseck;
	unsigned int	sitk_rtcalk;
	char		res2[0x10];
	unsigned int	sitk_piscrk;
	unsigned int	sitk_pitck;
	char		res3[0x38];
} sitk8xx_t;

/* Clocks and reset keys.
*/
typedef struct cark {
	unsigned int	cark_sccrk;
	unsigned int	cark_plprcrk;
	unsigned int	cark_rsrk;
	char		res[0x474];
} cark8xx_t;

/* The key to unlock registers maintained by keep-alive power.
*/
#define KAPWR_KEY	((unsigned int)0x55ccaa33)

/* LCD interface.  MPC821 Only.
*/
typedef struct lcd {
	unsigned short	lcd_lcolr[16];
	char		res[0x20];
	unsigned int	lcd_lccr;
	unsigned int	lcd_lchcr;
	unsigned int	lcd_lcvcr;
	char		res2[4];
	unsigned int	lcd_lcfaa;
	unsigned int	lcd_lcfba;
	char		lcd_lcsr;
	char		res3[0x7];
} lcd8xx_t;

/* I2C
*/
typedef struct i2c {
	unsigned char	i2c_i2mod;
	char		res1[3];
	unsigned char	i2c_i2add;
	char		res2[3];
	unsigned char	i2c_i2brg;
	char		res3[3];
	unsigned char	i2c_i2com;
	char		res4[3];
	unsigned char	i2c_i2cer;
	char		res5[3];
	unsigned char	i2c_i2cmr;
	char		res6[0x8b];
} i2c8xx_t;

/* DMA control/status registers.
*/
typedef struct sdma_csr {
	char		res1[4];
	unsigned int	sdma_sdar;
	unsigned char	sdma_sdsr;
	char		res3[3];
	unsigned char	sdma_sdmr;
	char		res4[3];
	unsigned char	sdma_idsr1;
	char		res5[3];
	unsigned char	sdma_idmr1;
	char		res6[3];
	unsigned char	sdma_idsr2;
	char		res7[3];
	unsigned char	sdma_idmr2;
	char		res8[0x13];
} sdma8xx_t;

/* Communication Processor Module Interrupt Controller.
*/
typedef struct cpm_ic {
	unsigned short	cpic_civr;
	char		res[0xe];
	unsigned int	cpic_cicr;
	unsigned int	cpic_cipr;
	unsigned int	cpic_cimr;
	unsigned int	cpic_cisr;
} cpic8xx_t;

/* Input/Output Port control/status registers.
*/
typedef struct io_port {
	unsigned short	iop_padir;
	unsigned short	iop_papar;
	unsigned short	iop_paodr;
	unsigned short	iop_padat;
	char		res1[8];
	unsigned short	iop_pcdir;
	unsigned short	iop_pcpar;
	unsigned short	iop_pcso;
	unsigned short	iop_pcdat;
	unsigned short	iop_pcint;
	char		res2[6];
	unsigned short	iop_pddir;
	unsigned short	iop_pdpar;
	char		res3[2];
	unsigned short	iop_pddat;
	char		res4[8];
} iop8xx_t;

/* Communication Processor Module Timers
*/
typedef struct cpm_timers {
	unsigned short	cpmt_tgcr;
	char		res1[0xe];
	unsigned short	cpmt_tmr1;
	unsigned short	cpmt_tmr2;
	unsigned short	cpmt_trr1;
	unsigned short	cpmt_trr2;
	unsigned short	cpmt_tcr1;
	unsigned short	cpmt_tcr2;
	unsigned short	cpmt_tcn1;
	unsigned short	cpmt_tcn2;
	unsigned short	cpmt_tmr3;
	unsigned short	cpmt_tmr4;
	unsigned short	cpmt_trr3;
	unsigned short	cpmt_trr4;
	unsigned short	cpmt_tcr3;
	unsigned short	cpmt_tcr4;
	unsigned short	cpmt_tcn3;
	unsigned short	cpmt_tcn4;
	unsigned short	cpmt_ter1;
	unsigned short	cpmt_ter2;
	unsigned short	cpmt_ter3;
	unsigned short	cpmt_ter4;
	char		res2[8];
} cpmtimer8xx_t;

/* Finally, the Communication Processor stuff.....
*/
typedef struct scc {		/* Serial communication channels */
	unsigned int	scc_gsmrl;
	unsigned int	scc_gsmrh;
	unsigned short	scc_pmsr;
	char		res1[2];
	unsigned short	scc_todr;
	unsigned short	scc_dsr;
	unsigned short	scc_scce;
	char		res2[2];
	unsigned short	scc_sccm;
	char		res3;
	unsigned char	scc_sccs;
	char		res4[8];
} scc_t;

typedef struct smc {		/* Serial management channels */
	char		res1[2];
	unsigned short	smc_smcmr;
	char		res2[2];
	unsigned char	smc_smce;
	char		res3[3];
	unsigned char	smc_smcm;
	char		res4[5];
} smc_t;

/* MPC860T Fast Ethernet Controller.  It isn't part of the CPM, but
 * it fits within the address space.
 */
typedef struct fec {
	unsigned int	fec_addr_low;		/* LS 32 bits of station address */
	unsigned short	fec_addr_high;		/* MS 16 bits of address */
	unsigned short	res1;
	unsigned int	fec_hash_table_high;
	unsigned int	fec_hash_table_low;
	unsigned int	fec_r_des_start;
	unsigned int	fec_x_des_start;
	unsigned int	fec_r_buff_size;
	unsigned int	res2[9];
	unsigned int	fec_ecntrl;
	unsigned int	fec_ievent;
	unsigned int	fec_imask;
	unsigned int	fec_ivec;
	unsigned int	fec_r_des_active;
	unsigned int	fec_x_des_active;
	unsigned int	res3[10];
	unsigned int	fec_mii_data;
	unsigned int	fec_mii_speed;
	unsigned int	res4[17];
	unsigned int	fec_r_bound;
	unsigned int	fec_r_fstart;
	unsigned int	res5[6];
	unsigned int	fec_x_fstart;
	unsigned int	res6[17];
	unsigned int	fec_fun_code;
	unsigned int	res7[3];
	unsigned int	fec_r_cntrl;
	unsigned int	fec_r_hash;
	unsigned int	res8[14];
	unsigned int	fec_x_cntrl;
	unsigned int	res9[0x1e];
} fec_t;

typedef struct comm_proc {
	/* General control and status registers.
	*/
	unsigned short	cp_cpcr;
	char		res1[2];
	unsigned short	cp_rccr;
	char		res2[6];
	unsigned short	cp_cpmcr1;
	unsigned short	cp_cpmcr2;
	unsigned short	cp_cpmcr3;
	unsigned short	cp_cpmcr4;
	char		res3[2];
	unsigned short	cp_rter;
	char		res4[2];
	unsigned short	cp_rtmr;
	char		res5[0x14];

	/* Baud rate generators.
	*/
	unsigned int	cp_brgc1;
	unsigned int	cp_brgc2;
	unsigned int	cp_brgc3;
	unsigned int	cp_brgc4;

	/* Serial Communication Channels.
	*/
	scc_t	cp_scc[4];

	/* Serial Management Channels.
	*/
	smc_t	cp_smc[2];

	/* Serial Peripheral Interface.
	*/
	unsigned short	cp_spmode;
	char		res6[4];
	unsigned char	cp_spie;
	char		res7[3];
	unsigned char	cp_spim;
	char		res8[2];
	unsigned char	cp_spcom;
	char		res9[2];

	/* Parallel Interface Port.
	*/
	char		res10[2];
	unsigned short	cp_pipc;
	char		res11[2];
	unsigned short	cp_ptpr;
	unsigned int	cp_pbdir;
	unsigned int	cp_pbpar;
	char		res12[2];
	unsigned short	cp_pbodr;
	unsigned int	cp_pbdat;
	char		res13[0x18];

	/* Serial Interface and Time Slot Assignment.
	*/
	unsigned int	cp_simode;
	unsigned char	cp_sigmr;
	char		res14;
	unsigned char	cp_sistr;
	unsigned char	cp_sicmr;
	char		res15[4];
	unsigned int	cp_sicr;
	unsigned int	cp_sirp;
	char		res16[0x10c];
	unsigned char	cp_siram[0x200];

	/* The fast ethernet controller is not really part of the CPM,
	 * but it resides in the address space.
	 */
	fec_t		cp_fec;
	char		res18[0x1000];

	/* Dual Ported RAM follows.
	 * There are many different formats for this memory area
	 * depending upon the devices used and options chosen.
	 */
	unsigned char	cp_dpmem[0x1000];	/* BD / Data / ucode */
	unsigned char	res19[0xc00];
	unsigned char	cp_dparam[0x400];	/* Parameter RAM */
} cpm8xx_t;

/* Internal memory map.
*/
typedef struct immap {
	sysconf8xx_t	im_siu_conf;	/* SIU Configuration */
	pcmconf8xx_t	im_pcmcia;	/* PCMCIA Configuration */
	memctl8xx_t	im_memctl;	/* Memory Controller */
	sit8xx_t	im_sit;		/* System integration timers */
	car8xx_t	im_clkrst;	/* Clocks and reset */
	sitk8xx_t	im_sitk;	/* Sys int timer keys */
	cark8xx_t	im_clkrstk;	/* Clocks and reset keys */
	lcd8xx_t	im_lcd;		/* LCD (821 only) */
	i2c8xx_t	im_i2c;		/* I2C control/status */
	sdma8xx_t	im_sdma;	/* SDMA control/status */
	cpic8xx_t	im_cpic;	/* CPM Interrupt Controller */
	iop8xx_t	im_ioport;	/* IO Port control/status */
	cpmtimer8xx_t	im_cpmtimer;	/* CPM timers */
	cpm8xx_t	im_cpm;		/* Communication processor */
} immap_t;

#endif /* __IMMAP_8XX__ */
