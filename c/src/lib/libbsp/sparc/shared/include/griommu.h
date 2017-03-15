/*
 *  GRIOMMU Driver Interface
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
 *  This driver controls the GRIOMMU device located 
 *  at an on-chip AMBA.
 */

#ifndef __GRIOMMU_H__
#define __GRIOMMU_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void griommu_register_drv(void);

#define GRIOMMU_ERR_OK 0
#define GRIOMMU_ERR_NOINIT -1
#define GRIOMMU_ERR_EINVAL -2
#define GRIOMMU_ERR_IMPLEMENTED -3
#define GRIOMMU_ERR_NOTFOUND -4

/* Size of APV (has to be divided by page size in bytes) */
#define GRIOMMU_APV_SIZE 0x20000000

/* Alignment of APV */
#define GRIOMMU_APV_ALIGN 0x10

/* IOMMU APV allocation helper functions */
extern void * griommu_apv_new(void);
extern void griommu_apv_delete(void * apv);

/* IOMMU Master Setup */

/* IOMMU Master find */
/*
 * GRIOMMU MASTER register fields 
 */
#define MASTER_VENDOR (0xff << MASTER_VENDOR_BIT)
#define MASTER_DEVICE (0xfff << MASTER_DEVICE_BIT)
#define MASTER_BS (0x1 << MASTER_BS_BIT)
#define MASTER_GROUP (0xf << MASTER_GROUP_BIT)

#define MASTER_VENDOR_BIT 24
#define MASTER_DEVICE_BIT 12
#define MASTER_BS_BIT 4
#define MASTER_GROUP_BIT 0

#define GRIOMMU_OPTIONS_BUS0 0
#define GRIOMMU_OPTIONS_BUS1 1
extern int griommu_master_setup(int master, int group, int options);
extern int griommu_master_find(int vendor, int device, int instance);
extern int griommu_master_info(int master, uint32_t * info);
#define griommu_get_master_vendor(info) \
	((info & MASTER_VENDOR) >> MASTER_VENDOR_BIT)
#define griommu_get_master_device(info) \
	((info & MASTER_DEVICE) >> MASTER_DEVICE_BIT)
#define griommu_get_master_routing(info) \
	((info & MASTER_BS) >> MASTER_BS_BIT)
#define griommu_get_master_group(info) \
	((info & MASTER_GROUP) >> MASTER_GROUP_BIT)

/* IOMMU Group Setup */
#define GRIOMMU_OPTIONS_GROUP_PASSTHROUGH 2
#define GRIOMMU_OPTIONS_GROUP_ENABLE 1
#define GRIOMMU_OPTIONS_GROUP_DISABLE 0
extern int griommu_group_setup(int group, void * apv, int options);
extern int griommu_group_info(int group, uint32_t * info);
#define GRIOMMU_OPTIONS_APV_ALLOW 0x1
#define GRIOMMU_OPTIONS_APV_DONTALLOW 0x0
extern int griommu_group_apv_init(int group, int options);
extern int griommu_group_apv_address_set(int group, uint32_t addr, int size, 
		int options);
extern int griommu_group_apv_page_set(int group, int index, int size, 
		int options);
extern int griommu_group_apv_flush(int group);

/* IOMMU Setup */
/*
 * GRIOMMU CTRL register fields 
 */
#define CTRL_PGSZ (0x7 << CTRL_PGSZ_BIT)
#define CTRL_LB (0x1 << CTRL_LB_BIT)
#define CTRL_SP (0x1 << CTRL_SP_BIT)
#define CTRL_ITR (0xf << CTRL_ITR_BIT)
#define CTRL_DP (0x1 << CTRL_DP_BIT)
#define CTRL_SIV (0x1 << CTRL_SIV_BIT)
#define CTRL_HPROT (0x3 << CTRL_HPROT_BIT)
#define CTRL_AU (0x1 << CTRL_AU_BIT)
#define CTRL_WP (0x1 << CTRL_WP_BIT)
#define CTRL_DM (0x1 << CTRL_DM_BIT)
#define CTRL_GS (0x1 << CTRL_GS_BIT)
#define CTRL_CE (0x1 << CTRL_CE_BIT)
#define CTRL_PM (0x3 << CTRL_PM_BIT)
#define CTRL_PM_APV (0x0 << CTRL_PM_BIT)
#define CTRL_PM_IOMMU (0x1 << CTRL_PM_BIT)
#define CTRL_EN (0x1 << CTRL_EN_BIT)

#define CTRL_PGSZ_BIT 18
#define CTRL_LB_BIT 17
#define CTRL_SP_BIT 16
#define CTRL_ITR_BIT 12
#define CTRL_DP_BIT 11
#define CTRL_SIV_BIT 10
#define CTRL_HPROT_BIT 8
#define CTRL_AU_BIT 7
#define CTRL_WP_BIT 6
#define CTRL_DM_BIT 5
#define CTRL_GS_BIT 4
#define CTRL_CE_BIT 3
#define CTRL_PM_BIT 1
#define CTRL_EN_BIT 0

#define GRIOMMU_OPTIONS_LOOKUPBUS_BUS0 0
#define GRIOMMU_OPTIONS_LOOKUPBUS_BUS1 CTRL_LB
#define GRIOMMU_OPTIONS_CACHE_DISABLE 0
#define GRIOMMU_OPTIONS_CACHE_ENABLE CTRL_CE
#define GRIOMMU_OPTIONS_GROUPADDRESSING_DISABLE 0
#define GRIOMMU_OPTIONS_GROUPADDRESSING_ENABLE CTRL_GS
#define GRIOMMU_OPTIONS_WPROTONLY_DISABLE 0
#define GRIOMMU_OPTIONS_WPROTONLY_ENABLE CTRL_WP
#define GRIOMMU_OPTIONS_AHBUPDATE_DISABLE 0
#define GRIOMMU_OPTIONS_AHBUPDATE_ENABLE CTRL_AU
#define GRIOMMU_OPTIONS_PREFETCH_DISABLE CTRL_DP
#define GRIOMMU_OPTIONS_PREFETCH_ENABLE 0
#define GRIOMMU_OPTIONS_PAGESIZE_4KIB 0
#define GRIOMMU_OPTIONS_PAGESIZE_8KIB (0x1 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_16KIB (0x2 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_32KIB (0x3 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_64KIB (0x4 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_128KIB (0x5 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_256KIB (0x6 << CTRL_PGSZ_BIT)
#define GRIOMMU_OPTIONS_PAGESIZE_512KIB (0x7 << CTRL_PGSZ_BIT)
extern int griommu_setup(int options);
extern int griommu_status(void);

#define GRIOMMU_MODE_IOMMU 1
#define GRIOMMU_MODE_GROUPAPV 0
extern int griommu_enable(int mode);
extern int griommu_disable(void);

/* IOMMU APV setup */
extern int griommu_apv_flush(void);
extern int griommu_apv_init(void * apv, int options);
extern int griommu_apv_address_set(void * apv, uint32_t addr, int size, 
		int options);
extern int griommu_apv_page_set(void * apv, int index, int size, int options);

/* GRIOMMU Interrupts */
/* Function Interrupt-Code ISR callback prototype.
 * arg	   - Custom arg provided by user
 * access  - AHB Access that failed
 * status  - Error status register of the GRIOMMU core
 */
typedef void (*griommu_isr_t)(void *arg, uint32_t access, uint32_t status);
#define GRIOMMU_INTERRUPT_ALL (0x2f << 0)
#define GRIOMMU_INTERRUPT_PARITY_ERROR (0x1 << 5)
#define GRIOMMU_INTERRUPT_FLUSH_COMPLETED (0x1 << 3)
#define GRIOMMU_INTERRUPT_FLUSH_START (0x1 << 2)
#define GRIOMMU_INTERRUPT_ACCESS_DENIED (0x1 << 1)
#define GRIOMMU_INTERRUPT_TRANSLATION_ERROR (0x1 << 0)
extern int griommu_isr_register(griommu_isr_t isr, void * arg, int options);
extern int griommu_isr_unregister(void);
extern int griommu_interrupt_unmask(int options);
extern int griommu_interrupt_mask(int options);

extern int griommu_error_status(uint32_t * access);

extern int griommu_print(void);

#ifdef __cplusplus
}
#endif

#endif /* __GRIOMMU_H__ */
