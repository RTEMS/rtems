#include <rtems.h>
#include <libcpu/spr.h>
#include <rtems/rtems/status.h>
#include <libcpu/mmu_support.h>
#include <libcpu/memoryprotection.h>
SPR_RW(SDR1);

rtems_status_code _CPU_Memory_management_Initialize(void)
{
  uint32_t pt_base,pt_end,cache_line_size;
  ppc_bsp_mm_mpe* pte;
  unsigned long msr;
  void *ea;
  
  pt_base = _read_SDR1() & 0xffff0000;
  pt_end = pt_base + (( ( _read_SDR1() & 0x000001ff)+1 )<<16);

  /* Switch MMU and other Interrupts off */
  msr = _read_MSR();
  _write_MSR(msr & ~ (MSR_EE | MSR_DR | MSR_IR)); 

  asm volatile ("sync":::"memory");
   /*rtems_cache_flush_entire_data for mpc6XX is not actually implemented*/
  /*rtems_cache_flush_entire_data();*/
  
  /*I am not quite sure is it proper to flush each line of entire address*
   * space for flushing cache.*/
  rtems_cache_flush_multiple_data_lines((void*)RamBase, (size_t)RamSize);

  for(pte = (ppc_bsp_mm_mpe*)pt_base; pte < (ppc_bsp_mm_mpe*)pt_end; pte+=1){
    pte->ptew0 &= ~0x80000000;
  }

  asm volatile(  \
    " sync ;  isync; \n"  \
    " tlbia ;  eieio;  \n"  \
    " tlbsync ;        \n"  \
    " sync ; isync;  \n"  \
    : : :"0");

  /* restore, i.e., switch MMU and IRQs back on */
  _write_MSR( msr );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Pte_Change_Attributes(rtems_memory_management_entry *mpe, uint32_t wimg, uint32_t pp)
{
  ppc_bsp_mm_mpe* pt_entry;
  uintptr_t ea, block_end;
  unsigned long msr;

  if(mpe->cpu_mpe == NULL) {
    printf("PTE not installed !");
    return RTEMS_UNSATISFIED;
  }
  
  if (wimg < 0 && pp < 0)
    return RTEMS_UNSATISFIED;

  ea = (uintptr_t) mpe->region.base;
  block_end = (uintptr_t)(mpe->region.base + mpe->region.bounds);
  rtems_cache_flush_multiple_data_lines(mpe->region.base, mpe->region.bounds);
  pt_entry = (ppc_bsp_mm_mpe *) mpe->cpu_mpe;

  /* Switch MMU and other Interrupts off */
  msr = _read_MSR();
  _write_MSR(msr & ~ (MSR_EE | MSR_DR | MSR_IR)); 

  for( ; ea < block_end; ea += RTEMS_MPE_PAGE_SIZE ) {
     pt_entry->ptew0 &= ~PTEW0_VALID;

     asm volatile ("sync":::"memory");

     pt_entry->ptew1 |= PTEW1_WIMG & wimg;

     pt_entry->ptew1 &= ~PTEW1_PROTP;
     pt_entry->ptew1 |= PTEW1_PROTP & pp;

     asm volatile ("sync; tlbie %0; eieio"::"r" (ea):"memory");
     pt_entry->ptew0 &= ~PTEW0_VALID;
     asm volatile ("tlbsync; sync":::"memory");
     pt_entry--;
  }
  /* restore, i.e., switch MMU and IRQs back on */
  _write_MSR( msr );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Install_MPE(
    rtems_memory_management_entry *mpe
) {
  ppc_bsp_mm_mpe* pt_entry, * ppteg,*spteg;
  int status;
  unsigned long msr;
  volatile uint32_t   sr_data, vsid, pi, hash1, hash2, api;
  volatile int ppteg_search_status, spteg_search_status;
  volatile int alut_access_attrb;
  uintptr_t ea, block_end;
  int wimg, pp;

  rtems_status_code retval = RTEMS_SUCCESSFUL;

  ea = (uintptr_t) mpe->region.base;
  block_end = (uintptr_t)(mpe->region.base + mpe->region.bounds);
  rtems_cache_flush_multiple_data_lines(mpe->region.base, mpe->region.bounds);

  for( ; ea < block_end; ea += RTEMS_MPE_PAGE_SIZE ) {
     
   /* Read corresponding SR Data */
  sr_data = _read_SR((void *) ea);

  /* Extract VSID */
  vsid = sr_data & SR_VSID;

  /* get page index (PI) from EA */
  pi = (ea >> 12) & 0x0000ffff;
  api = pi >> 10;

  /* Compute HASH 1 */
  hash1 = PTE_HASH_FUNC1(vsid, pi);

  /* Compute PTEG Address from the hash 1 value */
  get_pteg_addr(&ppteg, hash1);

  /* setting default permissions to no protection*/ 
  pp = _PPC_MMU_ACCESS_NO_PROT;
  mpe->cpu_mpe = BSP_ppc_add_pte(ppteg, spteg, vsid, pi, wimg, pp);

  if ( mpe->cpu_mpe == NULL ){
	retval = RTEMS_UNSATISFIED;
        break;
     }
   }  
  return retval;
}
                                  
rtems_status_code _CPU_Memory_management_Verify_size(
    size_t size
) {
   /* Check for invalid block size */
  if(((size % RTEMS_MPE_PAGE_SIZE) != 0) ||  (size == 0))
    return RTEMS_INVALID_SIZE; 

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Set_read_only(
   rtems_memory_management_entry  *mpe
){
  uint32_t pp;
  rtems_status_code status;

  pp = _PPC_MMU_ACCESS_READ_ONLY;
 
  status = _CPU_Pte_Change_Attributes(mpe,0x0,pp);
  if(status != RTEMS_SUCCESSFUL) 
   return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Set_write(
   rtems_memory_management_entry  *mpe
){
  uint32_t pp;
  rtems_status_code status;

  pp = _PPC_MMU_ACCESS_NO_PROT;

  status = _CPU_Pte_Change_Attributes(mpe,0x0,pp);
  if(status != RTEMS_SUCCESSFUL)
   return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;
}

