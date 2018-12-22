/* GRTC Telecommand (TC) decoder driver interface
 *
 * COPYRIGHT (c) 2007.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRTC_H__
#define __GRTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GRTC_IOC_UNUSED			0

/* Driver operation controlling commands */
#define GRTC_IOC_START			1
#define GRTC_IOC_STOP			2
#define GRTC_IOC_ISSTARTED		3
#define GRTC_IOC_SET_BLOCKING_MODE	4	/* Raw mode only */
#define GRTC_IOC_SET_TIMEOUT		5	/* Raw mode only */

#define GRTC_IOC_ADD_BUFF		16	/* Frame mode only */
#define GRTC_IOC_RECV			17	/* Frame mode only */

/* Available only in STOPPED mode */
#define GRTC_IOC_SET_MODE		32	/* Set frame mode (ioctl) or raw mode (read) */
#define GRTC_IOC_SET_BUF_PARAM		33
#define GRTC_IOC_SET_CONFIG		34
#define GRTC_IOC_POOLS_SETUP		35	/* Frame mode only */

/* Available in both running and stopped mode */
#define GRTC_IOC_GET_CONFIG		64
#define GRTC_IOC_GET_BUF_PARAM		65
#define GRTC_IOC_GET_HW_STATUS		66
#define GRTC_IOC_ASSIGN_FRM_POOL	67
#define GRTC_IOC_GET_CLCW_ADR		68	/* Get address of CLCWRx1 */
#define GRTC_IOC_GET_STATS		69	/* Get statistics, note that most of the stats are only avilable in FRAME mode */
#define GRTC_IOC_CLR_STATS		70	/* Clear statistics */

/* Available only in RUNNING mode */

/* Args to GRTC_IOC_GET_BUF_PARAMS */
#define GRTC_BUF_MAXLEN (0x100*1024)
#define GRTC_BUF_MASK 0xfffffc00
struct grtc_ioc_buf_params {
	unsigned int	length;		/* Length of new buffer in multiples of 1kbyte blocks */
	void		*custom_buffer;	/* If set zero driver will allocate with malloc, set LSB to 1 to indicate remote address */
};

/* Args to GRTC_IOC_SET_BLOCKING_MODE */
enum {
	GRTC_BLKMODE_POLL	= 0,	/* Never block (polling mode) */
	GRTC_BLKMODE_BLK	= 1,	/* Block until at least 1 byte can be read */
	GRTC_BLKMODE_COMPLETE	= 2	/* Block until all data requested has be read */
};

/* Argument of GRTC_IOC_SET_CONFIG and GRTC_IOC_GET_CONFIG 
 * Pointer to:
 */
struct grtc_ioc_config {
	int		psr_enable;
	int		nrzm_enable;
	int		pss_enable;
	int		crc_calc;	/* Enable Software CRC calculation (only Frame mode) */
};

/* Argument of GRTC_IOC_GET_HW_STATUS:
 * Pointer to a grtc_ioc_hw_status structure that will be filled
 * in by driver.
 */
struct grtc_ioc_hw_status {
	unsigned int	sir;
	unsigned int	far;
	unsigned int	clcw1;
	unsigned int	clcw2;
	unsigned int	phir;
	unsigned int	str;
};

struct grtc_hdr {
	unsigned short	flags_scid;
	unsigned short	vc_len;
	unsigned char	seqnum;
} __attribute__((packed));

/* Frame pool, all frames in pool have the same buffer length (frame mode only) */
struct grtc_frame {
	struct grtc_frame	*next;		/* Next frame in list */
	unsigned short		len;		/* Length of frame extracted */
	unsigned short		reserved;	/* Reserved */
	struct grtc_frame_pool	*pool;		/* The frame pool this frame belongs to */
	
	/* The Frame content */
	struct grtc_hdr		hdr;		/* Primary Header */
	unsigned char		data[3];	/* Frame payload */
} __attribute__((packed));

/* GRTC_IOC_RECV argument, single linked list of received frames */
struct grtc_list {
	struct grtc_frame	*head;		/* First frame in list */
	struct grtc_frame	*tail;		/* Last frame in list */
	int			cnt;		/* Number of frames in list */
};

struct grtc_ioc_pools_setup {
	unsigned int	pool_cnt;		/* Number of pools */
	unsigned int	pool_frame_len[1];	/* Array of 'pool_cnt' length: Frame length of frames in a pool 
						 * Lengths must be sorted, starting with the smallest frame pool.
						 */
};

struct grtc_ioc_assign_frm_pool {
	unsigned int		frame_len;	/* The length of the pool to insert the frame into */
	struct grtc_frame	*frames;	/* Frames to assign to a pool */
};

enum {
	GRTC_MODE_RAW   = 0,
	GRTC_MODE_FRAME = 1
};

/* TC driver stats collected during receiving. The statistics is only available 
 * in FRAME mode. In RAW mode the user interprets the incoming frames and is 
 * therefore responsible for generating the staticstics.
 */
struct grtc_ioc_stats {
	unsigned long long	frames_recv;		/* Total number of non-erroneous frames received */
	/* Errors related to incoming data */
	unsigned int		err;			/* total number of errors */
	unsigned int		err_hdr;		/* number of errors in Header */
	unsigned int		err_payload;		/* Number of errors in payload */
	unsigned int		err_ending;		/* Number of errors in end (Filler, end marker) */
	unsigned int		err_abandoned;		/* Number of abandoned frames, NOT IMPLEMENTED */
	/* Errors related to the handling of incoming frames */
	unsigned int		dropped;		/* Number of dropped frames TC driver */
	unsigned int		dropped_no_buf;		/* Number of dropped frame caused by no buffers were available */
	unsigned int		dropped_too_long;	/* Number of dropped frames that was larger than any buffer available for driver */
};

/* Register GRTC driver at driver manager */
void grtc_register_drv(void);

/* Register GRTC RMAP driver at driver manager */
void grtc_rmap_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif /* __GRTC_H__ */
