#ifndef _RTEMS_MMUSUPPORT_H
#define _RTEMS_MMUSUPPORT_H
/*
 *  * *  COPYRIGHT (c) 1989-2011.
 *   * *  On-Line Applications Research Corporation (OAR).
 *    * *
 *     * *  The license and distribution terms for this file may be
 *      * *  found in the file LICENSE in this distribution or at
 *       * *  http://www.rtems.com/license/LICENSE.
 *        * *
 *         * *
 *          * *  $Id$
 *           * */

#define ARM_MMU_AP_NOPR			0x03
#define ARM_MMU_AP_USER_READ_ONLY 	0x02
#define ARM_MMU_AP_USER_NO_ACCESS	0x01
#define ARM_MMU_AP_NO_ACCESS		0x00

#define ARM_MMU_cb 0x0 //cache and buffer disabled
#define ARM_MMU_cB 0x1 // cache disable and buffer enabled
#define ARM_MMU_WT 0x2 // Write through
#define ARM_MMU_WB 0x3 // Write Back

/* FIXME: ARM support various page sizes */
#define ARM_MMU_PAGE_SIZE  0x1000

/* This Control Structure applies for most ARM architectures */
typedef struct{
uint32_t vAddress;  /* virtual address for this page table */
uint32_t ptAdress;  /* physical address for page table lvl2 or lvl1 controling a region */
uint32_t pageNumbers /* Number of used PTEs from this page table */
uint32_t ptlvl1Adress; /* physical address for  level 1 page table */
uint8_t  type; /* lvl2 , lvl1 */
uint8_t  AP;
uint8_t  CB;
uint8_t  installed;
uint32_t domain;

} arm_bsp_mm_mpe;


/* Allocate a page table for one region 
 * FIXME: Specify maximum number of page table/regions here
 * or at conf file */
static inline PageTable* arm_PT_Allocate(void);

/* initialize page table with fault entries */
static inline rtems_status_code arm_PT_Initialize(arm_bsp_mm_mpe *);

/* Activating a region means that installing the corresponding lvl2 page table base address
 *  * at lvl1 pagetable, that is, the system is fully installed to generate exceptions and 
 *   * apply AP/CB when accessing that region according to its attributes. */
static inline rtems_status_code arm_PT_Activate(arm_bsp_mm_mpe *);

/* Add PTEs to a page table. Number of PTEs is determined according to region's size
 *  * Every page table controls one region, Installing a region means that allocating 
 *   * PTEs at CPU page tables, but not necessary activated */
static inline rtems_status_code arm_PT_Install(arm_bsp_mm_mpe *);

/* Deleting lvl2 page table base address from lvl1 page table */
static inline rtems_status_code arm_PT_UnInstall(arm_bsp_mm_mpe *);

/* Deleting a page table corresponds to delete a region */
static inline rtems_status_code arm_PT_Delete(arm_bsp_mm_mpe *);

/* Changing Page table attributes to new attributes */
static inline rtems_status_code arm_PT_Change_Attr(arm_bsp_mm_mpe *,uint32_t AP, uint32_t CB);

/* Verify that size must is multiple of page size */
inline rtems_status_code _CPU_Memory_management_Verify_size(uint32_t size);

#endif
