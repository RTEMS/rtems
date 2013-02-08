/*
 * COPYRIGHT (c) Hesham AL-Matary.
 * On-Line Applications Research Corporation (OAR).
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/arm-cp15.h>
#include <rtems/rtems/status.h>
#include "mmu_support.h"
#include <libcpu/memorymanagement.h>
extern uint32_t _ttbl_base;

/* Deleting MPE and freeing its Control block as well as uninstalling
 * mpe from HW and uninstall the high-level pointer to cpu_mpe.
 */
rtems_status_code _CPU_Memory_management_Delete_MPE(rtems_memory_management_entry *mpe)
{
  arm_bsp_mm_mpe *arm_mpe;
  arm_mpe = mpe->cpu_mpe;
  if ( arm_mpe == NULL )
    return RTEMS_NO_MEMORY;

  free(arm_mpe);

  return RTEMS_SUCCESSFUL;
}

/* Changing Page table attributes to new attributes */
rtems_status_code arm_Region_Change_Attr(arm_bsp_mm_mpe *mpe,uint32_t AP, uint32_t CB)
{
  mmu_lvl1_t     *lvl1_pt;
  int             sectionsNumber; /* 1MB sections */
  uint32_t        vAddress;
  int             PTEIndex;
  uint32_t        paddr;
  
  sectionsNumber = mpe->pagesNumber;
  
  lvl1_pt = (mmu_lvl1_t *) &_ttbl_base;
  PTEIndex = ((mpe->vAddress & 0xfff00000) >> 20);
  paddr = (mpe->vAddress & 0xfff00000);
  
  disable_mmu();

  int c = 0;
  int b = 0;

  switch (CB) {
        case ARM_MMU_cb:
            c = 0;
            b = 0;
            break;
        case ARM_MMU_cB:
            c = 0;
            b = 1;
            break;
        case ARM_MMU_WT:
            c = 1;
            b = 0;
            break;
        case ARM_MMU_WB:
            c = 1;
            b = 1;
            break;
        }
  /* Return AP/CB for this region to defaults */
  int i;
  for ( i = 0; i < sectionsNumber; i++) {
    uint32_t paddr_base = paddr;
    paddr_base = (i<<20) + paddr;
    lvl1_pt[PTEIndex++] = MMU_SET_LVL1_SECT(paddr_base,
                                            AP,
                                            0,
                                            c,
                                            b);
  }

#if DEBUG
  PTEIndex = ((mpe->vAddress & 0xfff00000) >> 20);
  uint32_t *PTE_debug = ((uint32_t *) ((mpe->ptAddress) + (mpe->pagesNumber * 4)));
  printk(" ~~~ Debug : Entered arm_Region_Change_Attr function succesfully and \n\
      changed the first PTE for region starting at base address %x with assigned \n\
     pagetable address at address %x is %x ~~~ \n",mpe->vAddress, mpe->ptAddress + (mpe->pagesNumber *4),\
     lvl1_pt[PTEIndex]);  
#endif

  mpe->ap = AP; /* Default when installing entry */
  mpe->cb = CB; /* Default */

  enable_mmu();

  return RTEMS_SUCCESSFUL;
}

/* Verify that size must is multiple of page size */
rtems_status_code _CPU_Memory_management_Verify_size(uint32_t size)
{
  if ( (size % MMU_SECT_SIZE) != 0)
    return RTEMS_INVALID_SIZE;

  return RTEMS_SUCCESSFUL;
}

/* Initialize first page table level with no protected entries */
rtems_status_code _CPU_Memory_management_Initialize(void)
{
  mmu_lvl1_t *lvl1_base;

  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  /* set manage mode access for all domains */
  arm_cp15_set_domain_access_control(0xffffffff);

  lvl1_base = (mmu_lvl1_t *) &_ttbl_base;

  /* set up the trans table */
  //
  //mmu_set_map_inval(lvl1_base);
  arm_cp15_set_translation_table_base(lvl1_base);

  /* fill level 1 pagetable with no protection slots, Cache through attributes
   * and 1:1 address mapping
   */
  int i;
  uint32_t paddr = 0x00000000;
  for (i = 0; i < (0x4000 / 4); i++) {
    paddr = i; /* i = 1MB page size */
    lvl1_base[i] = MMU_SET_LVL1_SECT((paddr << 20),
                                     ARM_MMU_AP_NOPR,
                                     0,
                                     1,
                                     0);
    }
  
  enable_mmu();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Install_MPE(
  rtems_memory_management_entry *mpe
)
{
  arm_bsp_mm_mpe *arm_mpe;
  mmu_lvl1_t     *lvl1_pt;
  int             sectionsNumber; /* 1MB sections */
  size_t          size; /* per Byte */
  uint32_t        vAddress;
  int             PTEIndex;
  uint32_t        paddr;

  lvl1_pt = (mmu_lvl1_t *) &_ttbl_base; 
  PTEIndex = ((((uint32_t)mpe->region.base) & 0xfff00000) >> 20);
  paddr = ((((uint32_t)mpe->region.base)) & 0xfff00000);  
  size = mpe->region.size;

  arm_mpe = (arm_bsp_mm_mpe *) malloc(sizeof(arm_bsp_mm_mpe));

  if ( arm_mpe == NULL )
    return RTEMS_NO_MEMORY;

  sectionsNumber = (size / MMU_SECT_SIZE);
 
  disable_mmu();

  /* Set AP for this region to NO ACCESS */

  int i;
  for ( i = 0; i < sectionsNumber; i++) {
    uint32_t paddr_base = paddr;
    paddr_base = (i<<20) + paddr;

    lvl1_pt[PTEIndex++] = MMU_SET_LVL1_SECT(paddr_base,
                                            ARM_MMU_AP_NO_ACCESS,
                                            0,
                                            1,
                                            0);
  }

  arm_mpe->vAddress = mpe->region.base;
  /* for level 1 page table ptAddress is the same as ptlvl1Address */
  arm_mpe->ptAddress = lvl1_pt;
  arm_mpe->ptlvl1Address = lvl1_pt;
  arm_mpe->pagesNumber = sectionsNumber;
  arm_mpe->type = LVL1_PT; /* Default value now */
  arm_mpe->ap   = ARM_MMU_AP_NO_ACCESS; /* Default when installing entry */
  arm_mpe->cb   = ARM_MMU_WT; /* Default */
  arm_mpe->domain = 0; 

  /* install a pointer to high-level API to bsp_mm_mpe */
  mpe->cpu_mpe = arm_mpe;
  
  enable_mmu();

  PTEIndex = ((arm_mpe->vAddress & 0xfff00000) >> 20);


  PTEIndex = ((arm_mpe->vAddress & 0xfff00000) >> 20);

#if DEBUG
    uint32_t *PTE_debug = ((uint32_t *) ((arm_mpe->ptAddress) + (arm_mpe->pagesNumber * 4)));
          printk(" ~~~ Debug : Entered _CPU_Memory_management_Install_MPE function succesfully and \n\
           installed the first PTE for region starting at base address %x with assigned \n\
           pagetable address at address %x is %x ~~~ \n",arm_mpe->vAddress, arm_mpe->ptAddress + (arm_mpe->pagesNumber *4),\
           lvl1_pt[PTEIndex + arm_mpe->pagesNumber - 1]);
#endif

  return RTEMS_SUCCESSFUL;
}

/* Uninstalling @mpe from level1 page table and return 
 * access/cache attributes to its defaults. Note that bsp MPE
 * will still exist even after uninstalling mpe. 
 */
rtems_status_code _CPU_Memory_management_UnInstall_MPE(
  rtems_memory_management_entry *mpe
)
{
  arm_bsp_mm_mpe *arm_mpe;
  mmu_lvl1_t     *lvl1_pt;
  int             sectionsNumber; /* 1MB sections */
  size_t          size; /* per Byte */
  uint32_t        vAddress;
  int             PTEIndex;
  uint32_t        paddr;

  arm_mpe = (arm_bsp_mm_mpe *) mpe->cpu_mpe;

  if ( arm_mpe == NULL )
    return RTEMS_UNSATISFIED;

  sectionsNumber = arm_mpe->pagesNumber;
  
  lvl1_pt = (mmu_lvl1_t *) &_ttbl_base;
  PTEIndex = ((((uint32_t)mpe->region.base) & 0xfff00000) >> 20);
  paddr = (((uint32_t)mpe->region.base) & 0xfff00000);
  
  disable_mmu();

  /* Return ap/CB for this region to defaults */
  int i;
  for ( i = 0; i < sectionsNumber; i++) {
    uint32_t paddr_base = paddr;
    paddr_base = (i<<20) + paddr;

    lvl1_pt[PTEIndex++] = MMU_SET_LVL1_SECT(paddr_base,
                                            ARM_MMU_AP_NO_ACCESS,
                                            0,
                                            1,
                                            0);
  }

  arm_mpe->ap   = ARM_MMU_AP_NO_ACCESS; /* Default */
  arm_mpe->cb   = ARM_MMU_WT; /* Default */

  enable_mmu();

  return RTEMS_SUCCESSFUL;
}

/* Disable MMU unit and caching (required before updating PTs)
 * include two steps:
 * 1- flushing caches and TLB (to ignore old caches page tables).
 * 2- turn of MMU and caching unit before editing page tables.
 */
rtems_status_code disable_mmu()
{
  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  /*  I & D caches turned off */
  arm_cp15_set_control(MMU_CTRL_DEFAULT |
                       MMU_CTRL_D_CACHE_DES |
                       MMU_CTRL_I_CACHE_DES |
                       MMU_CTRL_ALIGN_FAULT_EN |
                       MMU_CTRL_LITTLE_ENDIAN |
                       MMU_CTRL_MMU_DES);
  return RTEMS_SUCCESSFUL;
}

/* Re-enable MMU unit and caching (after updating PTs)
 * include two steps:
 * 1- flushing caches and TLB (to ignore old caches page tables).
 * 2- turn on MMU and caching unit after editing page tables.
 */
rtems_status_code enable_mmu()
{
  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  /*  I & D caches turned on */
  arm_cp15_set_control(MMU_CTRL_DEFAULT |
                       MMU_CTRL_D_CACHE_EN |
                       MMU_CTRL_I_CACHE_EN |
                       MMU_CTRL_ALIGN_FAULT_EN |
                       MMU_CTRL_LITTLE_ENDIAN |
                       MMU_CTRL_MMU_EN);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Set_read_only(
    rtems_memory_management_entry *mpe
){
  arm_bsp_mm_mpe *arm_mpe = (arm_bsp_mm_mpe *)(mpe->cpu_mpe);
  return arm_Region_Change_Attr(arm_mpe, ARM_MMU_AP_USER_READ_ONLY, ARM_MMU_WT);
}

rtems_status_code _CPU_Memory_management_Set_write(
    rtems_memory_management_entry *mpe
){
  arm_bsp_mm_mpe *arm_mpe = (arm_bsp_mm_mpe *)(mpe->cpu_mpe);
  return arm_Region_Change_Attr(arm_mpe, ARM_MMU_AP_NOPR, ARM_MMU_WT);
}

