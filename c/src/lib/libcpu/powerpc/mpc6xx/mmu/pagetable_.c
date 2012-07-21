#include <rtems.h>
#include <libcpu/spr.h>
#include <rtems/rtems/status.h>
#include <libcpu/mmu_support.h>
#include <libcpu/memoryprotection.h>
SPR_RW(SDR1);

/* Compute the primary hash from a VSID and a PI */
#define PTE_HASH_FUNC1(vsid, pi) (((vsid)^(pi))&(0x0007FFFF))

/* Compute the secondary hash from a primary hash */
#define PTE_HASH_FUNC2(hash1) ((~(hash1))&(0x0007FFFF))

static int pte_counter = 5;

static int
search_empty_pte_slot(libcpu_mmu_pte *pteg){
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


BSP_ppc_add_pte(libcpu_mmu_pte *ppteg,
    libcpu_mmu_pte *spteg,
                uint32_t vsid,
                uint32_t pi, 
                uint32_t wimg,
                uint32_t protp)
{
  int index;
  uint32_t hash, rpn, api;
  libcpu_mmu_pte* pteg;
  
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
    } else {
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
  return 0;
}

struct rtems_mm_attributes_struct { 
  uint32_t attr1;
 };
static int
search_valid_pte(libcpu_mmu_pte *pteg, uint32_t vsid, uint32_t api){
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

static void
get_pteg_addr(libcpu_mmu_pte** pteg, uint32_t hash){
  uint32_t masked_hash = 0x0;
  uint32_t htaborg, htabmask;
  htabmask = _read_SDR1() & 0x000001ff;
  htaborg = _read_SDR1() & 0xffff0000;
  masked_hash = ((htaborg >> 16) & 0x000001ff) | ((hash >> 10) & htabmask);
  *pteg = (libcpu_mmu_pte *)(htaborg | (masked_hash << 16) | (hash & 0x000003ff) << 6);
}


/*attribute check,  to make sure the attribute in ALUT is available 
*/
rtems_status_code _CPU_Pagetable_attr_Check(uint32_t attr )
{
  int pp,wimg;
  pp= attr&0xff;
  if(pp != 0xa && pp != 0xc && pp != 0xe && pp != 0xf )
    return RTEMS_UNSATISFIED;

  wimg = attr&0xff00;
  /*The combinations where WIM = 11x are not supported.*/
  if( (wimg& 0x3) ==0 )
    return RTEMS_UNSATISFIED;
    
  return RTEMS_SUCCESSFUL;
}

int
translate_access_attr(uint32_t attr, int * wimg, int * pp){
  int temp;
  temp = attr&0xff;
  if(  temp  == 0xa )
    *pp= _PPC_MMU_ACCESS_READ_ONLY;
  else if( temp == 0xc )
    *pp= _PPC_MMU_ACCESS_SUPERVISOR_ONLY;
  else if( temp == 0xe )
    *pp= _PPC_MMU_ACCESS_SUPERVISOR_WRITE_ONLY;
  else if( temp == 0xf )
    *pp= _PPC_MMU_ACCESS_NO_PROT;

  temp = (attr&0xff00)>8;
  *wimg = ((temp&1)<2) | ((temp&2)<2) | ((temp&4)>1) |((temp&8)>3) ; 
    
  return 0;
}

rtems_status_code _CPU_Memory_management_Initialize(void)
{
  
  uint32_t pt_base,pt_end,cache_line_size;
  libcpu_mmu_pte* pte;
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

  for(pte = (libcpu_mmu_pte*)pt_base; pte < (libcpu_mmu_pte*)pt_end; pte+=1){
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

rtems_status_code _CPU_Pte_Change_Attributes( uint32_t  ea,  int wimg, int pp)
{
  libcpu_mmu_pte* pt_entry, * ppteg,*spteg;
  int status;
  unsigned long msr;
  volatile uint32_t   sr_data, vsid, pi, hash1, hash2, api;
  volatile int ppteg_search_status, spteg_search_status;
  volatile int alut_access_attrb;
  rtems_memory_management_entry* mpe; 
  if (wimg < 0 && pp < 0)
    return RTEMS_UNSATISFIED;

  /* get effective address from DAR */
  //ea = _read_PPC_DAR();

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

  /* Search for PTE in group */
  search_ppteg:
  ppteg_search_status = search_valid_pte(ppteg, vsid, api);

  if (ppteg_search_status == -1){
    /* PTE Not found . Search in SPTEG */
    hash2 = PTE_HASH_FUNC2(hash1);
    get_pteg_addr(&spteg, hash2);
    spteg_search_status = search_valid_pte(spteg, vsid, api);
    if (ppteg_search_status == -1){
       /* PTE not found in second PTEG also */
      status = rtems_memory_management_find_entry((void *)ea, &mpe);
      if(status == RTEMS_SUCCESSFUL){
        /* Install MPE at CPU */
        status = BSP_ppc_add_pte(ppteg, spteg, vsid, pi, wimg, pp);
	/* go back and search again */
        goto search_ppteg;
        }
      else{
        printf("Unmapped Address when Installing MPE !!");
   
      }
    }
    else{ 
    //PTE found in SPTEG
    pt_entry = &spteg[spteg_search_status];
    }
  }else{
    pt_entry = &ppteg[ppteg_search_status];
  }
  
  /* Switch MMU and other Interrupts off */
  msr = _read_MSR();
  _write_MSR(msr & ~ (MSR_EE | MSR_DR | MSR_IR)); 

  pt_entry->ptew0 &= ~PTEW0_VALID;

  asm volatile ("sync":::"memory");

  if (wimg >= 0){
    pt_entry->ptew1 &= ~PTEW1_WIMG ;
    pt_entry->ptew1 |= PTEW1_WIMG & wimg ;
  }
  if (pp >= 0){
    pt_entry->ptew1 &= ~PTEW1_PROTP ;
    pt_entry->ptew1 |= PTEW1_PROTP & pp ;
  }

  asm volatile ("sync; tlbie %0; eieio"::"r" (ea):"memory");
  pt_entry->ptew0 &= ~PTEW0_VALID;
  asm volatile ("tlbsync; sync":::"memory");

  /* restore, i.e., switch MMU and IRQs back on */
  _write_MSR( msr );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Install_MPE(
    rtems_memory_management_entry *mpe
) {
  uintptr_t ea, block_end;
  uint32_t attr;
  int pagesize, wimg, pp;

  rtems_status_code retval = RTEMS_SUCCESSFUL;

  //ea = (uintptr_t)mpe->base;
  ea = (uintptr_t) mpe->region.base;
  //
  //    //block_end = (uintptr_t)(mpe->base + mpe->bounds);
  block_end = (uintptr_t)(mpe->region.base + mpe->region.bounds);
  pagesize = 0x1000; /* FIXME: 4K page */
  //
  rtems_cache_flush_multiple_data_lines(mpe->region.base, mpe->region.bounds);
  attr = mpe->permissions;
  translate_access_attr(attr, &wimg, &pp);
   for( ; ea < block_end; ea += pagesize ) {
     if ( (retval = _CPU_Pte_Change_Attributes(ea, wimg, pp)) != RTEMS_SUCCESSFUL )
        break;
   }  
	
  return retval;
}
                                  
rtems_status_code _CPU_Memory_management_Verify_size(
    size_t size
) {
   /* Check for invalid block size */
  if(((size % 0x1000) != 0) ||  (size == 0))
    return RTEMS_INVALID_SIZE; 

  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_management_Verify_permission(
   uint32_t permissions 
) {
  int pp,wimg;
  pp= permissions&0xff;
  if(pp != 0xa && pp != 0xc && pp != 0xe && pp != 0xf )
    return RTEMS_UNSATISFIED;

  wimg = permissions&0xff00;
  /*The combinations where WIM = 11x are not supported.*/
  if( (wimg& 0x3) ==0 )
    return RTEMS_UNSATISFIED;
    
  return RTEMS_SUCCESSFUL;
}

