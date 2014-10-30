/*
*  COPYRIGHT (c) 1989-2009.
*  On-Line Applications Research Corporation (OAR).
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.com/license/LICENSE.
*/

#include <rtems.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>
#include <bsp/irq_supp.h>
#include <rtems/powerpc/powerpc.h>
#include <rtems/irq.h>
#include <stdio.h> 
#include <rtems/asm.h>
#include <rtems/rtems/status.h>
#include <libcpu/memorymanagement.h>
#include "mmu_support.h"

SPR_RO(PPC_DAR);
SPR_RW(SDR1);
SPR_RO(DSISR);

//uint32_t BSP_MAX_ARENA = 64;

static int pte_counter = 5;

static int search_empty_pte_slot(ppc_bsp_mm_mpe *pteg){
  int i;
  for(i = 0; i < 8; i++) {
    if((pteg[i].ptew0 & PTEW0_VALID) != 0x80000000) {
      /* Found empty pte slot */
      return i;
    }
  }
  /* No PTE  free entry found, so rotate count and select a PTE */
  return -1;
}

static int search_valid_pte(ppc_bsp_mm_mpe *pteg, uint32_t vsid, uint32_t api){

  register int i;
  register uint32_t temp_vsid;
  register uint32_t temp_api;
    
  for(i = 0; i < 8; i++) {
    if((pteg[i].ptew0 & PTEW0_VALID) == 0x80000000) { 
     
      temp_api  = pteg[i].ptew0 & PTEW0_API;
      temp_vsid = (pteg[i].ptew0 & PTEW0_VSID) >> 7;
    
      if(temp_api == api && temp_vsid == vsid) { /* hit */
  	    return i;
      }  
    }
  }
  return -1;  /* Failed search */
}

ppc_bsp_mm_mpe*
BSP_ppc_add_pte(
  ppc_bsp_mm_mpe *ppteg,
  ppc_bsp_mm_mpe *spteg,
  uint32_t vsid,
  uint32_t pi, 
  uint32_t wimg,
  uint32_t protp)
{
  int index;
  uint32_t hash, rpn, api;
  ppc_bsp_mm_mpe* pteg;
  
  /* Search empty PTE slot in PPTEG */
  index = search_empty_pte_slot(ppteg);
  if (index != -1){
    pteg = ppteg;
    hash = 0;
  } else {
    /* Search Empty slot in  SPTEG */
    index = search_empty_pte_slot(spteg);
    if (index == -1) {
      /* Replace random pte entry depending on a counter */
      index = pte_counter;
      if (pte_counter == 7){
  	    pte_counter = 0;
      }
      pte_counter++;
      pteg = ppteg;
      hash = 0;
    }else {
      pteg = spteg;
      hash = 1;
    }
  }
    
  api = pi >> 10;
  rpn = pi;

  /* Clear the pte first . Invalidate */
  pteg[index].ptew0 = 0x00000000;
  pteg[index].ptew1 = 0x00000000;

  /* Update the PTE with new entry */
  pteg[index].ptew0 |= (vsid << 7) & PTEW0_VSID;
  pteg[index].ptew0 |= (hash << 6) & PTEW0_HASHF;
  pteg[index].ptew0 |= (api & PTEW0_API);
  pteg[index].ptew1 |= (rpn << 12);
  pteg[index].ptew1 |= (wimg << 3) & (PTEW1_WIMG);
  pteg[index].ptew1 |= (protp & PTEW1_PROTP);
  pteg[index].ptew0 |= PTEW0_VALID;
  return &pteg[index];
}

void
get_pteg_addr(ppc_bsp_mm_mpe** pteg, uint32_t hash){
  uint32_t masked_hash = 0x0;
  uint32_t htaborg, htabmask;
  htabmask = _read_SDR1() & 0x000001ff;
  htaborg = _read_SDR1() & 0xffff0000;
  masked_hash = ((htaborg >> 16) & 0x000001ff) | ((hash >> 10) & htabmask);
  *pteg = (ppc_bsp_mm_mpe *)(htaborg | (masked_hash << 16) | (hash & 0x000003ff) << 6);
}

/* This function shall be called upon exception on the DSISR
   register. depending on the type of exception appropriate action
   will be taken in this function. Most likely parameters for this
   function are
   - Exception Type (depending on DSISR[0..n]
   - address for which the transaltion failed
   - Anything else? */
static int
mmu_handle_dsi_exception(BSP_Exception_frame *f, unsigned vector){
  volatile uint32_t  ea, sr_data, vsid, pi, hash1, hash2, key, api,alut_access_attrb;
  volatile int ppteg_search_status, spteg_search_status;
  int status, pp, wimg;
  ppc_bsp_mm_mpe* ppteg;
  ppc_bsp_mm_mpe* spteg;
  volatile unsigned long cause, msr;
  rtems_memory_management_entry* alut_entry;
  
  /* Switch MMU and other Interrupts off */
  msr = _read_MSR();
  _write_MSR(msr & ~ (MSR_EE | MSR_DR | MSR_IR)); 

  #ifdef MMUS_DEBUG
  printk("~~~<debug information: get into the DSI exception handler!>~~~\n");
  #endif
 
  /* get effective address from DAR */
  ea = _read_PPC_DAR();
  cause = _read_DSISR();
 
  /* Read corresponding SR Data */
  sr_data = _read_SR((void *) ea);

  /* Extract VSID */
  vsid = sr_data & SR_VSID;

  /* get page index (PI) from EA */
  pi = (ea >> 12) & 0x0000ffff;
  api = pi >> 10;

  if(ea > (int)RamSize)
  {
    printk("Unexpected address 0x%x, RamSize = 0x%x\n",(unsigned int)ea,(unsigned int)RamSize);
    rtems_task_delete(RTEMS_SELF);
  }

  /* Compute HASH 1 */
  hash1 = PTE_HASH_FUNC1(vsid, pi);

  /* Compute PTEG Address from the hash 1 value */
  get_pteg_addr(&ppteg, hash1);

  /* Search for PTE in group */
  ppteg_search_status = search_valid_pte(ppteg, vsid, api);
  
  if (ppteg_search_status == -1){
  
    /* PTE Not found . Search in SPTEG */
    hash2 = PTE_HASH_FUNC2(hash1);
    get_pteg_addr(&spteg, hash2);
    spteg_search_status = search_valid_pte(spteg, vsid, api);
    if (spteg_search_status == -1){
      /* PTE not found in second PTEG also */
      printk("No CPU MPE found for this address");
    } else {
      /* PTE found in second group */
      /* Code for determining access attribute goes here */
      key = ((sr_data & SR_KP) && MSR_PR) || ((sr_data & SR_KS) && ~(MSR_PR));
      pp = spteg[spteg_search_status].ptew1 & PTEW1_PROTP;
      if (((key && (pp == 1 || pp == 0 || pp == 3)) ||
        (~(key) && (pp == 3)) ) &&(cause & 0x02000000)){
        /* Write access denied */
        printk("Error:Write access denied. Please check the access\
        	attribute of address 0x%x! RTEMS delete self\n", ea);
        rtems_task_delete(RTEMS_SELF); 
      } else if ((key && (pp == 0)) && (~cause & 0x02000000)) {
        /* Read access denied */
        printk("Error:Read access denied. Please check the access\
                attribute of address 0x%x! RTEMS delete self\n", ea);
        rtems_task_delete(RTEMS_SELF); 
      } else {
        /* Access permitted */
      }
    }
  } else { 
    /* PTE found in primary group itself */
    /* Code for determining attribute goes here */
    key = ((sr_data & SR_KP) && MSR_PR) || ((sr_data & SR_KS) && ~(MSR_PR));
    pp = ppteg[ppteg_search_status].ptew1 & PTEW1_PROTP;
    if (((key && (pp == 1 || pp == 0 || pp == 3)) ||
      (~(key) && (pp == 3))) &&(cause & 0x02000000)){
        /* Write access denied */
         printk("Error:Write access denied. Please check the access\
	            	 attribute of address 0x%x! RTEMS delete self\n", ea);
        rtems_task_delete(RTEMS_SELF); 
    } else if ((key && (pp == 0)) && (~cause & 0x02000000)) {
      /* Read access denied */
      printk("Error:Read access denied. Please check the access\
	            attribute of address 0x%x! RTEMS delete self\n", ea);
      rtems_task_delete(RTEMS_SELF); 
    } else {
      /* Access permitted */
    }
  }
  
  /* Before returning turn on MMU */
  _write_MSR( msr );
  return 0;
}

static int
mmu_handle_tlb_dlmiss_exception(BSP_Exception_frame *f, unsigned vector){
  printk("DL TLB MISS Exception \n");
  return 0;
}

static int
mmu_handle_tlb_dsmiss_exception(BSP_Exception_frame *f, unsigned vector){
  printk("DS TLB MISS Exception \n");
  return 0;
}

void
mmu_irq_init(void){
  uint32_t i;
  ppc_exc_set_handler(ASM_PROT_VECTOR, mmu_handle_dsi_exception);
  ppc_exc_set_handler(ASM_60X_DLMISS_VECTOR, mmu_handle_tlb_dlmiss_exception);
  ppc_exc_set_handler(ASM_60X_DSMISS_VECTOR, mmu_handle_tlb_dsmiss_exception);

  /* Initialise segment registers */
  for (i=0; i<16; i++) 
    _write_SR(i|0x60000000 , (void *)(i<<28 ));

  /* Set up SDR1 register for page table address */
  _write_SDR1((unsigned long) 0x00FF0000);

  _CPU_Memory_management_Initialize();
}

/* Make a BAT entry (either IBAT or DBAT entry) Parameters to pass
   would be EA, PA, Block Length, Access Bits */
void
mmu_make_bat_entry(void){

}

/* Same thing as above but for Instruction access */
void
mmu_handle_isi_exception(void){

}
