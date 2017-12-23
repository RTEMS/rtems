/* GRTM Telemetry (TM) driver interface
 * 
 * COPYRIGHT (c) 2007.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRTM_H__
#define __GRTM_H__

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GRTM_IOC_UNUSED			0

/* Driver operation controlling commands */
#define GRTM_IOC_START			1
#define GRTM_IOC_STOP			2
#define GRTM_IOC_ISSTARTED		3
#define GRTM_IOC_SET_BLOCKING_MODE	4
#define GRTM_IOC_SET_TIMEOUT		5

/* Available only in STOPPED mode */
#define GRTM_IOC_SET_CONFIG		32

/* Available in both running and stopped mode */
#define GRTM_IOC_RECLAIM		64
#define GRTM_IOC_GET_CONFIG		65
#define GRTM_IOC_GET_HW_IMPL		66
#define GRTM_IOC_GET_HW_STATUS		67	/* Not implemented */
#define GRTM_IOC_GET_OCFREG		68
#define GRTM_IOC_GET_STATS		69
#define GRTM_IOC_CLR_STATS		70

/* Available only in RUNNING mode */
#define GRTM_IOC_SEND			96

/* Args to GRTC_IOC_SET_BLOCKING_MODE */
enum {
	GRTM_BLKMODE_POLL	= 0,	/* Never block (polling mode) */
	GRTM_BLKMODE_BLK	= 1,	/* Block until at least 1 byte can be read */
};

/* Reed Solomon Encoder implemented */
enum {
	GRTM_RS_IMPL_NONE	= 0,
	GRTM_RS_IMPL_E16	= 1,	/* E16 */
	GRTM_RS_IMPL_E8		= 2,	/* E8 */
	GRTM_RS_IMPL_BOTH	= 3	/* Both E8 and E16 */

};

struct grtm_ioc_hw {
	char		cs;		/* Sub Carrier */
	char		sp;		/* Split-Phase Level */
	char		ce;
	char		nrz;
	char		psr;
	char		te;
	unsigned char	rsdep;
	unsigned char	rs;
	char		aasm;
	char		fecf;
	char		ocf;
	char		evc;
	char		idle;
	char		fsh;
	char		mcg;
	char		iz;
	char		fhec;
	char		aos;
	char		cif;
	char		ocfb;
	
	unsigned short	blk_size;	/* Block Size */
	unsigned short	fifo_size;	/* FIFO Size */
	
};

/* Driver Mode */
enum {
	GRTM_MODE_TM		= 0,	/* TM */
	GRTM_MODE_AOS		= 1	/* AOS */
};

/* Physical layer Options */
#define GRTM_IOC_PHY_SCF	(1<<15)		/* Sub Carrier Fall */
#define GRTM_IOC_PHY_SF		(1<<31)		/* Symbol Fall */

/* Coding Sub-layer Options */
#define GRTM_IOC_CODE_SC	(1<<0)		/* Enable Sub Carrier modulation */
#define GRTM_IOC_CODE_SP	(1<<1)		/* Enable Split-Phase (SP) level modulation */
#define GRTM_IOC_CODE_CE	(1<<5)		/* Enable Convolutional Encoding */
#define GRTM_IOC_CODE_NRZ	(1<<6)		/* Enable Non-Return-to-Zero mark encoding */
#define GRTM_IOC_CODE_PSR	(1<<7)		/* Enable Pseudo-Randomizer */
#define GRTM_IOC_CODE_RS8	(1<<11)		/* Reed-solomon Encoder to use: 0=E16, 1=E8 */
#define GRTM_IOC_CODE_RS	(1<<15)		/* Enable Reed-Solomon Encoder */
#define GRTM_IOC_CODE_AASM	(1<<16)		/* Enable Alternative attached synchronization marker */
#define GRTM_IOC_CODE_ALL	(GRTM_IOC_CODE_SC|GRTM_IOC_CODE_SP|GRTM_IOC_CODE_CE| \
				GRTM_IOC_CODE_NRZ|GRTM_IOC_CODE_PSR|GRTM_IOC_CODE_RS8|\
				GRTM_IOC_CODE_RS|GRTM_IOC_CODE_AASM)

enum {
	GRTM_CERATE_00		= 0,	/* Rate 1/2, no puncturing */
	GRTM_CERATE_02		= 2,	/* Rate 1/2, punctured */
	GRTM_CERATE_04		= 4,	/* Rate 2/3, punctured */
	GRTM_CERATE_05		= 5,	/* Rate 3/4, punctured */
	GRTM_CERATE_06		= 6,	/* Rate 5/6, punctured */
	GRTM_CERATE_07		= 7,	/* Rate 7/8, punctured */
};

/* Options for Generating all frames */
#define GRTM_IOC_ALL_FHEC	0x01	/* Enable Frame Header Error Control (Only AOS) */
#define GRTM_IOC_ALL_FECF	0x02	/* Enable Transfer Frame CRC */
#define GRTM_IOC_ALL_IZ		0x04	/* Enable Insert Zone */
#define GRTM_IOC_ALL_ALL	(GRTM_IOC_ALL_FHEC|GRTM_IOC_ALL_FECF|GRTM_IOC_ALL_IZ)

/* Master Frame Generation Options */
#define GRTM_IOC_MF_OW		0x01	/* Over Write OCF bits 16 and 17 */
#define GRTM_IOC_MF_OCF		0x02	/* Enable Operation Control Field (OCF) for master channel */
#define GRTM_IOC_MF_FSH		0x04	/* Enable MC_FSH for master channel */
#define GRTM_IOC_MF_MC		0x08	/* Enable Master channel counter generation */
#define GRTM_IOC_MF_ALL		(GRTM_IOC_MF_OW|GRTM_IOC_MF_OCF|GRTM_IOC_MF_FSH|GRTM_IOC_MF_MC)

/* Idle Frames Generation Options */
#define GRTM_IOC_IDLE_MC 	0x01	/* Enable Master Channel (MC) counter generation (TM Only) */
#define GRTM_IOC_IDLE_VCC	0x02	/* Enable Virtual Channel counter cycle generation (AOS Only)*/
#define GRTM_IOC_IDLE_FSH	0x04	/* Enable Frame Secondary Header (FSH) for idle frames (TM Only) */
#define GRTM_IOC_IDLE_EVC	0x08	/* Enable Extended Virtual Channel Counter Generation */
#define GRTM_IOC_IDLE_OCF	0x10	/* Enable OCF/CLCW in idle frame */
#define GRTM_IOC_IDLE_EN	0x20	/* Enable Idle frame generation */
#define GRTM_IOC_IDLE_ALL	(GRTM_IOC_IDLE_MC|GRTM_IOC_IDLE_VCC|GRTM_IOC_IDLE_FSH| \
				GRTM_IOC_IDLE_EVC|GRTM_IOC_IDLE_OCF|GRTM_IOC_IDLE_EN)

/* Argument of GRTM_IOC_SET_CONFIG and GRTM_IOC_GET_CONFIG.
 * Driver and Hardware configuration.
 *
 * Pointer to:
 */
struct grtm_ioc_config {

	/* Mode AOS or TM */
	unsigned char	mode;		/* 0=TM, 1=AOS */

	unsigned short	frame_length;	/* Length of every frame transmitted */
	unsigned short	limit;		/* Number of data bytes fetched by DMA before transmission starts */
	unsigned int	as_marker;	/* Attached Synchronization Marker */
	
	/* Physical layer options */
	unsigned short	phy_subrate;	/* Sub Carrier rate - sub carrier devision factor - 1 */
	unsigned short	phy_symbolrate;	/* Symbol Rate division factor - 1 */
	unsigned char	phy_opts;	/* Mask of GRTM_IOC_PHY_XXXX */

	/* Coding sub-layer Options */
	unsigned char	code_rsdep;	/* Coding sub-layer Reed-Solomon interleave depth (3-bit) */
	unsigned char	code_ce_rate;	/* Convolutional encoding rate, select one of GRTM_CERATE_00 ... GRTM_CERATE_07 */
	unsigned char	code_csel;	/*   */
	unsigned int	code_opts;	/* Mask of GRTM_IOC_CODE_XXXX */

	/* All Frames Generation */
	unsigned char	all_izlen;	/* FSH/IZ Length (5-bit) */
	unsigned char	all_opts;	/* Mask of GRTM_IOC_ALL_XXXX */

	/* Master Frame Generation */
	unsigned char	mf_opts;	/* Mask of GRTM_IOC_MF_XXXX */

	/* Idle frame Generation */
	unsigned short	idle_scid;
	unsigned char	idle_vcid;
	unsigned char	idle_opts;	/* Mask of GRTM_IOC_IDLE_XXXX */

	/* Interrupt options */
	unsigned int	enable_cnt;	/* Number of frames in between Interrupt is generated, Zero disables interrupt */
	int		isr_desc_proc;	/* Enable ISR to process descriptors */
	int		blocking;	/* Blocking mode select (POLL,BLK..) */
	rtems_interval	timeout;	/* Blocking mode timeout */
};

struct grtm_frame;

struct grtm_list {
	struct grtm_frame *head;	/* First Frame in list */
	struct grtm_frame *tail;	/* Last Frame in list */
};

#define GRTM_FLAGS_SENT		0x01
#define GRRM_FLAGS_ERR		0x02

#define GRTM_FLAGS_TRANSLATE	(1<<31)	/* Translate frame payload address from CPU address to remote bus (the bus GRTM is resident on) */
#define GRTM_FLAGS_TRANSLATE_AND_REMEMBER	(1<<30) /* As GRTM_FLAGS_TRANSLATE, however if the translated payload address equals the payload address
							 * the GRTM_FLAGS_TRANSLATE_AND_REMEMBER bit is cleared and the GRTM_FLAGS_TRANSLATE bit is set */
#define GRTM_FLAGS_COPY_DATA	(1<<29)	/* Where available: Transfer Frame payload to target, may be used for SpaceWire, where the GRTM driver transfer 
					 * the payload to a buffer on the SpaceWire target. 
					 */

#define GRTM_FLAGS_TS		(1<<14)
#define GRTM_FLAGS_VCE		(1<<9)
#define GRTM_FLAGS_MCB		(1<<8)
#define GRTM_FLAGS_FSHB		(1<<7)
#define GRTM_FLAGS_OCFB		(1<<6)
#define GRTM_FLAGS_FHECB	(1<<5)
#define GRTM_FLAGS_IZB		(1<<4)
#define GRTM_FLAGS_FECFB	(1<<3)

#define GRTM_FLAGS_MASK		(GRTM_FLAGS_TS|GRTM_FLAGS_VCE|GRTM_FLAGS_MCB|\
				 GRTM_FLAGS_FSHB|GRTM_FLAGS_OCFB|GRTM_FLAGS_FHECB|\
				 GRTM_FLAGS_IZB|GRTM_FLAGS_FECFB)

/* The GRTM software representation of a Frame */
struct grtm_frame {
	/* Options and status */
	unsigned int		flags;		/* bypass options, and sent/error status */
	
	struct grtm_frame	*next;		/* Next packet in chain */

	unsigned int		*payload;	/* The Headers and Payload,  Frame data and header must be word aligned */
};

#define FRAME_SIZE(payloadlen)	(sizeof(struct grtm_frame)+payloadlen)

struct grtm_ioc_stats {
	unsigned long long	frames_sent;
	unsigned int		err_underrun;
	unsigned int		err_tx;
	unsigned int		err_ahb;
	unsigned int		err_transfer_frame;
};

/* Register GRTM driver at driver manager */
void grtm_register_drv(void);

/* Register GRTM RMAP driver at driver manager */
void grtm_rmap_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif /* __GRTM_H__ */
