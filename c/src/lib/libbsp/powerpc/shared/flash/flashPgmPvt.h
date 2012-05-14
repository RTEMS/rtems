#ifndef FLASH_GLUE_INTERFACE_H
#define FLASH_GLUE_INTERFACE_H


/* Trivial flash programmer (for restrictions see below)     */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2006 */

/* DO NOT INCLUDE THIS HEADER FROM APPLICATION CODE          */

/*
 * Glue interface -- to be used only internally by BSP
 * and chip drivers:
 * - BSP provides info about what chip drivers to use
 *   as well as 'wiring' info (how many devices are
 *   operated in parallel etc).
 * - Chip drivers provide low-level 'methods' / 'ops'
 *   for performing basic operations which are used
 *   by the code in 'flash.c'.
 */

/* To keep things simple, this API makes a few assumptions about the
 * hardware:
 *
 * - devices operate with 16-bit data width
 * - two devices are used in parallel (stride 4) to
 *   provide 32-bit data. I.e., the devices are
 *   organized like this:
 *   unsigned short flash[FLASH_SIZE][2];
 * - no endianness issues (i.e., flash endianness ==  CPU endianness)
 * - fixed block size
 * - fixed buffer size
 * - all devices in a bank are identical
 * - NOT THREAD SAFE; no locking scheme is implemented.
 * - cannot copy within same flash bank.
 * - write-timeout uses polling/busy-wait
 *
 * FIXME: code should be revised to remove assumptions on stride and 16-bit
 *        width to make it more generic.
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <stdint.h>

#define NumberOf(arr)		(sizeof(arr)/sizeof(arr[0]))

#define FLASH_STRIDE(b)     4 /* bytes; currently fixed */
#define FLASH_WIDTH(b)      ((b)->width)
#define FLASH_NDEVS(b)      (FLASH_STRIDE(b)/FLASH_WIDTH(b))

/* Type declarations */

/* Registers          */
typedef uint8_t           _u8_a_t  __attribute__((may_alias));
typedef uint16_t          _u16_a_t __attribute__((may_alias));
typedef uint32_t          _u32_a_t __attribute__((may_alias));

/* Register addresses */
typedef volatile _u8_a_t  *A8;
typedef volatile _u16_a_t *A16;
typedef volatile _u32_a_t *A32;

struct flash_bank_ops;

/*
 * Description of a flash bank. Multiple
 * devices that are used in parallel to
 * make up words of FLASH_STRIDE bytes
 * are a 'physical' bank.
 *
 * A bank can even be a 'logical' bank
 * if it includes chip-select logic, i.e.,
 * int can contain multiple adjacent
 * 'physical' banks
 *
 * The BSP must provide an array of 'bankdesc'
 * structs and it must initialize the fields
 *
 *   'start'
 *     size of bank; may be set to zero to instruct
 *     the driver to scan a bank of 'max_size' for
 *     devices (i.e., bank may not be fully populated)
 *   'max_size'
 *     size of fully populated bank (defines address range
 *     that is scanned for devices).
 *     If 'max_size' is negative then scanning starts from
 *     the top rather than from the bottom.
 *   'width'
 *     width of a single device (in bytes). E.g., if
 *     2 16-bit devices are used to form a (ATM fixed)
 *     stride of 4 then 'width = 2'. If four 8-bit
 *     devices are employed then 'width=1'.
 *   'knownVendors'
 *     array of vendors descriptions to use for scanning
 *     the bank.
 *
 */
struct bankdesc {
	uint32_t	start;		/* start of bank (CPU address)              */
	uint32_t	size;		/* in bytes (figured out automatically)     */
	int			max_size;	/* in case multiple banks are adjacent;
	                         * if max_size < 0 then the bank is scanned
							 * backwards (from top->bottom) for devices
							 */
	int         width;      /* FIXME there might be implicit assumptions still
	                         * that width == 2
							 */
	struct vendesc        *knownVendors;
	/* TODO: we assume identical devices within a bank... */

	/* The next three variables cache information obtained
	 * from the applicable vendor and device descriptions.
	 * They are written by BSP_flashCheckId().
	 */
	uint32_t	fblksz;     /* block size in bytes; includes counting
	                         * parallel 16-bit devices, i.e., if a
							 * single device has a block-size of xxx
							 * then fblksz = xxx*ndevs.
							 */
	struct devdesc        *dd;
	struct flash_bank_ops *ops;
};

struct devdesc {
	uint32_t	id;     /* numerical ID (matched against
	                     * ID read from device).
						 */
	char		*name;  /* informational name           */
	uint32_t	size;	/* bytes                        */
	uint32_t	bufsz;	/* size of write buffer (bytes) */
	uint32_t	fblksz; /* sector/block size (bytes)    */
};

struct vendesc {
	uint32_t	id;     /* numerical ID (matched against
	                     * ID read from device).
						 */
	char		*name;  /* informational name */

	                    /* array of supported devices;
						 * the 'ops' specified below
						 * are used to access these devices
						 */
	struct devdesc	      *known_devs;
						/* access methods for talking to
						 * devices associated with this
						 * vendor description.
						 */
	struct flash_bank_ops *ops;
};

/* Device Access Methods ('ops'); these must be
 * implemented by low-level chip drivers
 */

struct flash_bank_ops {
/* Read vendor/device ID; Return 0 on success, nonzero if unable to read id */
	int            (*get_id)(struct bankdesc *b, uint32_t addr, uint32_t *pVendorId, uint32_t *pDeviceId);
/* Unlock block holding 'addr'ess
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

	void           (*unlock_block)(struct bankdesc *b, uint32_t addr);
/* Lock block holding 'addr'ess
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

	void           (*lock_block)(struct bankdesc *b, uint32_t addr);
/* Erase single block holding 'addr'ess. The routine may
 * assume that the address is block/sector aligned.
 *
 * RETURNS: zero on error, device status on failure.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */
	int            (*erase_block)(struct bankdesc *b, uint32_t addr);
/* Query the status of the device and assert it's readiness
 * leave off in array-reading mode.
 *
 * RETURNS: 0 on success, error status (result of status query) on error.
 *
 *   NOTES: - error message is printed to stderr.
 *          - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */
	uint32_t       (*check_ready)(struct bankdesc *b, uint32_t addr);
/* Dump status bits (F_CMD_RD_STA results);
 * 'verbose' prints non-error bits, too
 */
	void           (*print_stat)(struct bankdesc *b, uint32_t sta, int verbose);
/* Switch to array mode; 'addr' can be assumed to be stride-aligned */
	void           (*array_mode)(struct bankdesc *b, uint32_t addr);
/* Write N bytes from 'src' to flash:
 * 'src[0] .. src[N-1]' -> addr[0]..addr[N-1].
 * N may be assumed to be a multiple of 'stride'
 * RETURNS: failure status or zero on success.
 */
	uint32_t       (*write_line)(struct bankdesc *b, uint32_t addr, char *src, uint32_t N);
};

/* BSP ops (detect banks, handle write-protection on board);
 * these must be implemented by the BSP.
 */

struct flash_bsp_ops {
/* Return descriptor for bank # 'bank' or NULL (invalid arg) */
	struct bankdesc *(*bankcheck)(int bank, int quiet);
/* set (enbl:1), clear (enbl:0) or query (enbl:-1)
 * on-board write protection.
 *
 * RETURNS 0 on success, nonzero on error.
 */
	int              (*flash_wp)(int bank, int enbl);
/* read a running us clock (for polling timeout) */
	uint32_t         (*read_us_timer)();
};

/* This must be provided by the BSP */
extern struct flash_bsp_ops BSP_flashBspOps;

/* Available low-level flash drivers, so far */
extern struct vendesc        BSP_flash_vendor_intel[];
extern struct vendesc        BSP_flash_vendor_spansion[];

#endif
