/*
 *  GRLIB L2CACHE Driver
 *
 *  COPYRIGHT (c) 2017
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  OVERVIEW
 *  ========
 *  This driver controls the L2CACHE device located 
 *  at an on-chip AMBA.
 */

#ifndef __L2CACHE_H__
#define __L2CACHE_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void l2cache_register_drv(void);

#define L2CACHE_ERR_OK 0
#define L2CACHE_ERR_NOINIT -1
#define L2CACHE_ERR_EINVAL -2
#define L2CACHE_ERR_TOOMANY -3
#define L2CACHE_ERR_ERROR -4

/* L2C Flush options */
#define L2CACHE_OPTIONS_FLUSH_WAIT (0x1 << 2)
#define L2CACHE_OPTIONS_FLUSH_INVALIDATE (0x3 << 0)
#define L2CACHE_OPTIONS_FLUSH_WRITEBACK (0x2 << 0)
#define L2CACHE_OPTIONS_FLUSH_INV_WBACK (0x1 << 0)
#define L2CACHE_OPTIONS_FLUSH_NONE (0 << 0)

/* L2C Status */
#define L2CACHE_STATUS_ENABLED 1
#define L2CACHE_STATUS_SPLIT_ENABLED (0x1 << 1)
#define L2CACHE_STATUS_EDAC_ENABLED (0x1 << 2)
#define L2CACHE_STATUS_REPL (0x3 << L2CACHE_STATUS_REPL_BIT)
#define L2CACHE_STATUS_REPL_BIT 3
#define L2CACHE_STATUS_WRITETHROUGH (0x1 << 5)
#define L2CACHE_STATUS_LOCK (0xf << L2CACHE_STATUS_LOCK_BIT)
#define L2CACHE_STATUS_LOCK_BIT 6
#define L2CACHE_STATUS_SCRUB_ENABLED (0x1 << 10)
#define L2CACHE_STATUS_INT (0xf << L2CACHE_STATUS_INT_BIT)
#define L2CACHE_STATUS_INT_BIT 11
#define L2CACHE_STATUS_INT_BCKEND (0x1 << 11)
#define L2CACHE_STATUS_INT_WPHIT (0x1 << 12)
#define L2CACHE_STATUS_INT_UEE (0x1 << 13)
#define L2CACHE_STATUS_INT_CEE (0x1 << 14)
#define L2CACHE_STATUS_SCRUB_DELAY (0xffff << L2CACHE_STATUS_SCRUB_DELAY_BIT)
#define L2CACHE_STATUS_SCRUB_DELAY_BIT 15
#define L2CACHE_STATUS_SIGN_BIT 31

/* status helper macros */
#define L2CACHE_ENABLED(status) (status & L2CACHE_STATUS_ENABLED)
#define L2CACHE_DISABLED(status) (!(status & L2CACHE_STATUS_ENABLED))
#define L2CACHE_SPLIT_ENABLED(status) (status & L2CACHE_STATUS_SPLIT_ENABLED)
#define L2CACHE_SPLIT_DISABLED(status) \
	(!(status & L2CACHE_STATUS_SPLIT_ENABLED))
#define L2CACHE_EDAC_ENABLED(status) (status & L2CACHE_STATUS_EDAC_ENABLED)
#define L2CACHE_EDAC_DISABLED(status) (!(status & L2CACHE_STATUS_EDAC_ENABLED))
#define L2CACHE_REPL(status) \
	((status & L2CACHE_STATUS_REPL) >> L2CACHE_STATUS_REPL_BIT)
#define L2CACHE_WRITETHROUGH(status) (status & L2CACHE_STATUS_WRITETHROUGH)
#define L2CACHE_WRITEBACK(status) (!(status & L2CACHE_STATUS_WRITETHROUGH))
#define L2CACHE_LOCKED_WAYS(status) \
	((status & L2CACHE_STATUS_LOCK) >> L2CACHE_STATUS_LOCK_BIT)
#define L2CACHE_SCRUB_ENABLED(status) (status & L2CACHE_STATUS_SCRUB_ENABLED)
#define L2CACHE_SCRUB_DISABLED(status) \
	(!(status & L2CACHE_STATUS_SCRUB_ENABLED))
#define L2CACHE_SCRUB_DELAY(status) \
	((status & L2CACHE_STATUS_SCRUB_DELAY) >> L2CACHE_STATUS_SCRUB_DELAY_BIT)
#define L2CACHE_INT_ENABLED(status) (status & L2CACHE_STATUS_INT)
#define L2CACHE_INT_DISABLED(status) (!(status & L2CACHE_STATUS_INT))
extern int l2cache_status(void);

/* L2C Setup */
extern int l2cache_enable(int flush);
extern int l2cache_disable(int flush);

extern int l2cache_split_enable(void);
extern int l2cache_split_disable(void);

extern int l2cache_edac_enable(int flush);
extern int l2cache_edac_disable(int flush);

extern int l2cache_scrub_enable(int delay);
extern int l2cache_scrub_disable(void);
extern int l2cache_scrub_line(int way, int index);

extern int l2cache_writethrough(int flush);
extern int l2cache_writeback(int flush);

#define L2CACHE_OPTIONS_REPL_INDEX_WAY_BIT (2)
#define L2CACHE_OPTIONS_REPL_MASTERIDX_MOD (3 << 0)
#define L2CACHE_OPTIONS_REPL_MASTERIDX_IDX (2 << 0)
#define L2CACHE_OPTIONS_REPL_RANDOM (1 << 0)
#define L2CACHE_OPTIONS_REPL_LRU (0 << 0)
extern int l2cache_replacement(int options, int flush);

/* L2C Flush */
extern int l2cache_flush(int flush);
extern int l2cache_flush_address(uint32_t addr, int size, int flush);
extern int l2cache_flush_line(int way, int index, int flush);
extern int l2cache_flush_way(int way, int flush);

/* L2C Lock way */
#define L2CACHE_OPTIONS_DIRTY (0x1 << 2)
#define L2CACHE_OPTIONS_VALID (0x1 << 1)
#define L2CACHE_OPTIONS_FETCH (0x1 << 0)
#define L2CACHE_OPTIONS_DISABLE 2
#define L2CACHE_OPTIONS_ENABLE 1
#define L2CACHE_OPTIONS_NONE 0
extern int l2cache_lock_way(uint32_t tag, int options, int flush, int enable);
extern int l2cache_unlock(void);

/* L2C Fill a way */
extern int l2cache_fill_way(int way, uint32_t tag, int options, int flush);

/* L2C MTRR */
#define L2CACHE_OPTIONS_MTRR_ACCESS_WRITETHROUGH (0x1 << 2)
#define L2CACHE_OPTIONS_MTRR_ACCESS_UNCACHED (0x0 << 2)
#define L2CACHE_OPTIONS_MTRR_WRITEPROT_ENABLE (0x1 << 1)
#define L2CACHE_OPTIONS_MTRR_WRITEPROT_DISABLE (0x0 << 1)
extern int l2cache_mtrr_enable(int id, uint32_t addr, uint32_t mask, 
		int options, int flush);
extern int l2cache_mtrr_disable(int id);

/* L2C Debug print */
extern int l2cache_print(void);

/* L2C Interrupts */
/* Function Interrupt-Code ISR callback prototype.
 * arg	   - Custom arg provided by user
 * addr    - Cacheline addr that generated the error
 * status  - Error status register of the L2CACHE core
 */
typedef void (*l2cache_isr_t)(void *arg, uint32_t addr, uint32_t status);
#define L2CACHE_INTERRUPT_ALL (0xf << 0)
#define L2CACHE_INTERRUPT_BACKENDERROR (0x1 << 3)
#define L2CACHE_INTERRUPT_WPROTHIT (0x1 << 2)
#define L2CACHE_INTERRUPT_UNCORRERROR (0x1 << 1)
#define L2CACHE_INTERRUPT_CORRERROR (0x1 << 0)
extern int l2cache_isr_register( l2cache_isr_t isr, void * arg, int options);
extern int l2cache_isr_unregister(void);
extern int l2cache_interrupt_mask(int options);
extern int l2cache_interrupt_unmask(int options);

/* L2C error interface */
#define L2CACHE_STATUS_MULTIPLEERRORS 2
#define L2CACHE_STATUS_NEWERROR 1
#define L2CACHE_STATUS_NOERROR 0
extern int l2cache_error_status(uint32_t * addr, uint32_t * status);

/*#define TEST_L2CACHE*/
#ifdef TEST_L2CACHE
/* Used for internal testing */
/*
 * L2CACHE Tag private data struture
 */
struct l2cache_tag {
	uint32_t tag;
	int valid;
	int dirty;
	int lru;
};

/*
 * L2CACHE Line private data struture
 */
struct l2cache_dataline {
	uint32_t data[16];
	int words;
};
extern int l2cache_get_index( uint32_t addr);
extern uint32_t l2cache_get_tag( uint32_t addr);

extern int l2cache_diag_tag( int way, int index, struct l2cache_tag * tag);
extern int l2cache_diag_line( int way, int index, 
		struct l2cache_dataline * dataline);

#define L2CACHE_HIT 1
#define L2CACHE_MISS 0
extern int l2cache_lookup(uint32_t addr, int * way);

extern int l2cache_error_inject_address( uint32_t addr, uint32_t mask);
#endif /* TEST_L2CACHE */

#ifdef __cplusplus
}
#endif

#endif /* __L2CACHE_H__ */
