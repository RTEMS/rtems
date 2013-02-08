#ifndef __MMU_SUPPORT_H_
#define __MMU_SUPPORT_H_

#include <stdint.h>

#define MMU_CTRL_MMU_EN             (1 << 0)
#define MMU_CTRL_MMU_DES            (0 << 0)
#define MMU_CTRL_ALIGN_FAULT_EN     (1 << 1)
#define MMU_CTRL_D_CACHE_EN         (1 << 2)
#define MMU_CTRL_D_CACHE_DES        (0 << 2)
#define MMU_CTRL_DEFAULT            (0xf << 3)
#define MMU_CTRL_LITTLE_ENDIAN      (0 << 7)
#define MMU_CTRL_BIG_ENDIAN         (1 << 7)
#define MMU_CTRL_SYS_PROT           (1 << 8)
#define MMU_CTRL_ROM_PROT           (1 << 9)
#define MMU_CTRL_I_CACHE_EN         (1 << 12)
#define MMU_CTRL_I_CACHE_DES        (0 << 12)
#define MMU_CTRL_LOW_VECT           (0 << 13)
#define MMU_CTRL_HIGH_VECT          (1 << 13)

#define ARM_MMU_AP_NOPR             0x03
#define ARM_MMU_AP_USER_READ_ONLY   0x02
#define ARM_MMU_AP_USER_NO_ACCESS   0x01
#define ARM_MMU_AP_NO_ACCESS        0x00

#define ARM_MMU_cb 0x0 //cache and buffer disabled
#define ARM_MMU_cB 0x1 // cache disable and buffer enabled
#define ARM_MMU_WT 0x2 // Write through
#define ARM_MMU_WB 0x3 // Write Back

#define MMU_SET_FAULT (0x0) // Fault entry 

/* FIXME: ARM supports various page sizes */
#define ARM_MMU_PAGE_SIZE  0x1000
#define MMU_SECT_SIZE      0x100000

/* Set lvl1 PTE to point to the base of a page table mapping
 * 4K pages 
 */
#define MMU_SET_LVL1_DIR_4K_PAGES(lvl2_base, dom) \
           ((((lvl2_base) & 0xfffffc00)) |     \
           (((dom)&0xf)<<5)                  |     \
           0x11)                

#define MMU_SET_LVL2_PTE_4KB(paddr,ap,c,b) \
                            (((paddr)&0xfffff000)   | \
                            ((ap)<<10)      | \
                            ((ap)<<8)       | \
                            ((ap)<<6)       | \
                            ((ap)<<4)       | \
                            ((c)<<3)        | \
                            ((b)<<2)        | \
                            0x2)

#define MMU_SET_LVL1_SECT(addr, ap, dom, ce, be) \
                         ((((addr) & 0xfff00000)) |     \
                         ((ap) << 10)            |     \
                         ((dom) << 5)            |     \
                         ((ce) << 3)             |     \
                         ((be) << 2)             |     \
                         0x12)

/* TODO: Add other page sizes */
typedef enum { LVL1_PT, LVL2_PT } pt_type;

typedef uint32_t mmu_lvl1_t;
typedef uint32_t mmu_lvl2_t;

/* This Control Structure applies for most ARM architectures */
typedef struct{
uint32_t vAddress;
uint32_t ptAddress;  /* physical address for page table lvl2 or lvl1 controlling a region */
uint32_t ptlvl1Address; /* physical address for  level 1 page table */
uint16_t pagesNumber; /* Number of used PTEs from this page table */
pt_type  type; /* lvl2 , lvl1 */
uint8_t  ap;
uint8_t  cb;
uint8_t domain;
} arm_bsp_mm_mpe;

rtems_status_code enable_mmu();

rtems_status_code disable_mmu();

#endif 
