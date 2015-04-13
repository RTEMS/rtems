/*  GR1553B BM driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR1553BM_H__
#define __GR1553BM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Register GR1553B driver needed by BM driver */
extern void gr1553bm_register(void);

struct gr1553bm_entry {
	uint32_t time;	/* bit31=1, bit 30=0 */
	uint32_t data;	/* bit31=0, bit 30=0 */
};

#define GR1553BM_ERROPTS_MANL 0x02
#define GR1553BM_ERROPTS_UDWL 0x04
#define GR1553BM_ERROPTS_IMCL 0x08
#define GR1553BM_ERROPTS_ALL  0x0e

/* Function used to implement a custom copy routine.
 * Returns number of bytes the desctionation address 
 * should be incremented with.
 *
 * \param dst        Optional Destination address
 * \param src        Source DMA address
 * \param nentires   Number of entries to be processed.
 * \param data       Custom Data (set by config)
 */
typedef int (*bmcopy_func_t)(
	unsigned int dst,
	struct gr1553bm_entry *src,
	int nentries,
	void *data
	);

/* IRQ function callback, called on BM DMA error */
typedef void (*bmisr_func_t)(void *bm, void *data);

/* BM driver configuration */
struct gr1553bm_config {

	/*** Time options ***/

	/* 8-bit time resolution, the BM will update the time according
	 * to this setting. 0 will make the time tag be of highest
	 * resolution (no division), 1 will make the BM increment the
	 * time tag once for two time ticks (div with 2), etc.
	 */
	uint8_t time_resolution;

	/* Enable Time Overflow IRQ handling. Setting this to 1 
	 * makes the driver to update the 64-bit time by it self,
	 * it will use time overflow IRQ to detect when the 64-bit
	 * time counter must be incremented.
	 *
	 * If set to zero, the driver expect the user to call
	 * gr1553bm_time() regularly, it must be called more often
	 * than the time overflows to avoid an incorrect time.
	 */
	int time_ovf_irq;



	/*** Filtering options ***/

	/* Bus error log options 
	 *
	 * bit0,4-31 = reserved, set to zero
	 * Bit1 = Enables logging of Invalid mode code errors
	 * Bit2 = Enables logging of Unexpected Data errors
	 * Bit3 = Enables logging of Manchester/parity errors
	 */
	unsigned int filt_error_options;

	/* RT Address filtering bit mask. Each bit enables (if set)
	 * logging of a certain RT sub address. Bit 31 enables logging
	 * of broadcast messages.
	 */
	unsigned int filt_rtadr;

	/* RT Subaddress filtering bit mask, bit definition:
	 *  31:     Enables logging of mode commands on subadr 31
	 *  1..30:  BitN enables/disables logging of RT subadr N
	 *  0:      Enables logging of mode commands on subadr 0
	 */
	unsigned int filt_subadr;

	/* Mode code Filter, is written into "BM RT Mode code filter"
	 * register, please see hardware manual for bit declarations.
	 */
	unsigned int filt_mc;



	/*** Buffer options ***/

	/* Size of buffer in bytes, must be aligned to 8-byte 
	 * The size is limited to max 4Mb.
	 */
	unsigned int buffer_size;

	/* Custom buffer, must be aligned to 8-byte and be of buffer_size
	 * length. If NULL dynamic memory allocation is used.
	 */
	void *buffer_custom;

	/* Custom Copy function, may be used to implement a more 
	 * effective way of copying the DMA buffer. For example
	 * the DMA log may need to be compressed before copied
	 * onto a storage, this function can be used to avoid an
	 * extra copy.
	 */
	bmcopy_func_t copy_func;

	/* Optional Custom Data passed on to copy_func() */
	void *copy_func_arg;



	/*** Interrupt options ***/

	/* Custom DMA error function, note that this function is called
	 * from Interrupt Context. Set to NULL to disable this callback.
	 */
	bmisr_func_t dma_error_isr;

	/* Optional Custom Data passed on to dma_error_isr() */
	void *dma_error_arg;
};

/* Open BM device by instance number (minor)
 *
 * The return value is used as input parameter in all other function calls
 * in the A
 */
extern void *gr1553bm_open(int minor);

/* Close previously opened Bm device */
extern void gr1553bm_close(void *bm);

/* Configure the BM driver before starting */
extern int gr1553bm_config(void *bm, struct gr1553bm_config *cfg);

/* Start logging */
extern int gr1553bm_start(void *bm);

/* Get 64-bit 1553 Time. Low 24-bit time is acquired from BM hardware,
 * the MSB is taken from a software counter internal to the driver. The
 * counter is incremented every time the Time overflows by:
 *  - using "Time overflow" IRQ if enabled in user configuration
 *  - by checking IRQ flag (IRQ disabled), it is required that user
 *    calls this function before the next time overflow.
 *
 * The BM timer is limited to 24-bits, in order to handle overflows
 * correctly and maintain a valid time an Interrupt handler is used
 * or this function must be called when IRQ is not used.
 *
 * Update software time counters and return the current time.
 */
extern void gr1553bm_time(void *bm, uint64_t *time);

/* Return zero when logging has not been started, non-zero when logging
 * has been started
 */
extern int gr1553bm_started(void *bm);

/* Check how many entries are currently stored in the BM Log DMA-area */
extern int gr1553bm_available(void *bm, int *nentries);

/* Stop logging */
extern void gr1553bm_stop(void *bm);

/* Read a maximum number of entries from LOG buffer. This function
 * must be 
 *
 * Arguments
 *  bm           - Private pointer returned by gr1553bm_open()
 *  dst          - Address where log data is written
 *  max          - (IN/OUT) Maximum number of entires, when successfull
 *                 the number of entries actually written is stored
 *                 into the address of max.
 *
 * Result
 *  0   = success
 *  -1  = fail. (may be due to BM logging not started)
 */
extern int gr1553bm_read(void *bm, struct gr1553bm_entry *dst, int *max);

#ifdef __cplusplus
}
#endif

#endif /* __GR1553BM_H__ */
