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

#define ARM_MMU_PAGE_SIZE  0x1000

/* This Control Structure applies for most ARM architectures */
typedef struct{
uint32_t vAddress;  /* virtual address for this page table */
uint32_t ptAdress;  /* physical address for this page table lvl2 or lvl1 */
uint32_t ptlvl1Adress; /* physical address for  level 1 page table */
uint32_t type; /* lvl2 , lvl1 */
uint32_t domain;
} PageTable;

/* Region Control Block, Controls a region with a given size
 * contails page table for this region, and number of pages 
 * as well as access permissions */
typedef struct{
uint32_t 	vAddress; /* Virtual address for this region to be mapped */
uint32_t 	pageSize;   /*FIXME: default 4KB page size for this region */ 
uint32_t 	pageNumbers; /* Number of page table entries for this region */
uint8_t		AP;	/* Access permissions for this Region */
uint8_t		CB;	/* CACHE and BUFFER attributes for this Region */
bool	 	installed; /* Whether the corresponding page table is active or not */
PageTable       pt;  /*  Page table that controls this region */ 
} arm_bsp_mm_mpe;
#endif
