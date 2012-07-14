/*COPYRIGHT (c) 1989-2011.
*  On-Line Applications Research Corporation (OAR).
*  *
*  *  The license and distribution terms for this file may be
*  *  found in the file LICENSE in this distribution or at
*  *  http://www.rtems.com/license/LICENSE.
*  *
*  *  $Id$
*  */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/thread.h>
#include <rtems/error.h>
#include <rtems/rtems/cache.h>
//#include <libcpu/memoryprotection.h>
#include "libmmu.h"


/* create a alut table instance */
  rtems_mprot_alut the_rtems_mprot_alut;



/*****************************************************
 * * flush  the cache and tlb, update the pagetable of the mpe *
 * *  block address space*
 * *****************************************************/
void rtems_memory_management_update_entry(rtems_memory_management_entry*  mpe)
{
  void * ea, *block_end;
  int pagesize, cache_attr, mprot_attr;
  ea = mpe->region.base;
  block_end = mpe->region.base+ mpe->region.bounds;
  pagesize = 0x1000;

  
  rtems_cache_flush_multiple_data_lines(mpe->region.base, mpe->region.bounds);
  asm volatile(" sync; isync");
  
  translate_access_attr(mpe->permissions, &cache_attr, &mprot_attr);
  for(  ; ea<block_end;ea+=pagesize)
    rtems_pagetable_update_permissions((uint32_t)ea, cache_attr, mprot_attr);
  
  return ;
}

/*****************************************************
 * * initialization of the ALUT (Access Look up Table )  *
 * *****************************************************/
rtems_status_code rtems_memory_management_initialize ( void)
{
  rtems_mprot_alut* the_alut= &the_rtems_mprot_alut;
  int i;

  _Chain_Initialize(&(the_alut->ALUT_idle), the_alut->entries, 
    RTEMS_MPROT_ALUT_SIZE, sizeof(rtems_memory_management_entry) );

  _Chain_Initialize_empty(&(the_alut->ALUT_mappings));
  
  for(i=0;i<RTEMS_MPROT_ALUT_SIZE; i++)
  {
    the_alut->entries[i].region.bounds = 0;
    the_alut->entries[i].region.base = (void*)0;
    the_alut->entries[i].permissions = 0;
  }

  the_alut->rtems_mprot_default_permissions = RTEMS_MPROT_DEFAULT_ATTRIBUTE;
  
  rtems_pagetable_initialize();
  return RTEMS_SUCCESSFUL;
}


/*****************************************************
 * * Linear search for the element emtry in the alut for*
 * * the address range under which it falls             *
 * *****************************************************/
rtems_status_code rtems_memory_management_find_entry(
  void* const addr, 
  rtems_memory_management_entry** p_ret)
{
  rtems_mprot_alut* alut_p;
  rtems_memory_management_entry* current;
  alut_p = &the_rtems_mprot_alut;

  if( 0==p_ret   )   
    return RTEMS_INVALID_ADDRESS;

  current = (rtems_memory_management_entry* )rtems_chain_first( &alut_p->ALUT_mappings);
  while(!rtems_chain_is_tail(&(alut_p->ALUT_mappings),&current->node)){
    if( (current->region.base <= addr ) && \
      ((current->region.base+current->region.bounds )>addr) ){
      *p_ret = current;
      return RTEMS_SUCCESSFUL;
    }
    current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  return RTEMS_UNSATISFIED;
}

/****************************************************
 * * Add a ALUT new entry to ALUT
 * ****************************************************/
rtems_status_code rtems_memory_management_create_entry(
  rtems_memory_management_region_descriptor region,
  const uint32_t permissions,
  rtems_memory_management_entry** p_ret)
{
  rtems_memory_management_entry* current;
  rtems_mprot_alut* alut_p;
  rtems_status_code status;
  alut_p = &the_rtems_mprot_alut;

  /* Check for invalid block size */

  status = rtems_memory_management_verify_size(region.bounds);
  if( status != RTEMS_SUCCESSFUL )
    return RTEMS_INVALID_NUMBER;

  /*advanced attribute check , need to make sure the attribute of the *
 *   * new block is available. the check is a CPU related function.*/
  status = rtems_memory_management_verify_permissions( permissions);
  if( status != RTEMS_SUCCESSFUL)
    return RTEMS_INVALID_NUMBER;

  _Thread_Disable_dispatch();
  /* Check for address map overlaps */
  current = (rtems_memory_management_entry* )rtems_chain_first( &alut_p->ALUT_mappings);
  while(!rtems_chain_is_tail(&alut_p->ALUT_mappings,&current->node)){
    if( !((current->region.base >= ( region.base + region.bounds) ) ||\
      ((current->region.base +current->region.bounds ) <= region.base) )){
        _Thread_Enable_dispatch();
        return RTEMS_INVALID_ADDRESS;
      }
    current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  
   /* Check for ALUT full condition and get a valid empty entry */
  if(rtems_chain_is_empty(&(alut_p->ALUT_idle)) )
  {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
  
  current = (rtems_memory_management_entry* )rtems_chain_get_unprotected(&(alut_p->ALUT_idle));
  
 /* Append entry to the ALUT */
  current->region.base = region.base;
  current->region.bounds = region.bounds;
  current->permissions = permissions;
  rtems_chain_append_unprotected( &alut_p->ALUT_mappings, &current->node);
  
  /* for the new entry block , the attribute may be different from the previous value*
 *   *  so update the related cache tlb and pagetable entry*/
  rtems_memory_management_update_entry(current);

  _Thread_Enable_dispatch();
  
  *p_ret = current;
  return RTEMS_SUCCESSFUL;
}

/*****************************************************
 * * Delete an ALUT entry                                         *
 * *****************************************************/
rtems_status_code rtems_memory_management_delete_entry(
  rtems_memory_management_entry* const mpe)
{
  rtems_mprot_alut* alut_p;
  rtems_memory_management_entry*  current;
  alut_p = &the_rtems_mprot_alut;

  _Thread_Disable_dispatch();
  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  /*make sure the mpe point to a mapped entry */
  current = (rtems_memory_management_entry* )rtems_chain_first( &alut_p->ALUT_mappings);
  while(!rtems_chain_is_tail(&alut_p->ALUT_mappings, &current->node)){
    if( rtems_chain_are_nodes_equal(&current->node,&mpe->node )){

      rtems_chain_extract_unprotected( &mpe->node );

      /* for the delete entry block , the attribute should be changed default access attribute*
 *        *  so  update the related cache tlb and pagetable entry*/
      mpe->permissions = rtems_memory_management_get_default_permissions();
      rtems_memory_management_update_entry(mpe);

      
      mpe->region.bounds = 0;
      mpe->region.base = 0;
      mpe->permissions = 0;
      rtems_chain_append_unprotected( &alut_p->ALUT_idle, &current->node);

      _Thread_Enable_dispatch();  
      return RTEMS_SUCCESSFUL;
    }else
      current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  
   _Thread_Enable_dispatch();
  return RTEMS_INVALID_ADDRESS;
}

/*****************************************************
 * * reset the attribute of the specific  ALUT entry        *
 * *****************************************************/
rtems_status_code rtems_memory_management_set_permissions(
  rtems_memory_management_entry* const mpe,
  const uint32_t new_permissions,
  uint32_t* old_permissions )
{
  rtems_mprot_alut* alut_p;
  alut_p = &the_rtems_mprot_alut;
  rtems_status_code status;

  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();
  if(0 == mpe->region.bounds )
  {
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  /*advanced attribute check , need to make sure the attribute of the *
 *   * new block is available. the check is a CPU related function.*/
  status = rtems_memory_management_verify_permissions( new_permissions);
  if( status != RTEMS_SUCCESSFUL)
    return RTEMS_INVALID_NUMBER;

  *old_permissions = mpe->permissions;
  if( *old_permissions == new_permissions)
  {
    _Thread_Enable_dispatch();
    return RTEMS_SUCCESSFUL;
  }

  mpe->permissions = new_permissions;
  
  rtems_memory_management_update_entry(mpe);
  
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}


/*****************************************************
 * * get  the attribute of the specific  ALUT entry                        *
 * * this method may be used by the Exception handler function. *
 * *****************************************************/

rtems_status_code rtems_memory_management_get_permissions(
  rtems_memory_management_entry* const mpe,
  uint32_t*  permissions)
{
  rtems_mprot_alut* alut_p;
  alut_p = &the_rtems_mprot_alut;

  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();
  if(0 == mpe->region.bounds )
  {
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  *permissions = mpe->permissions;
  _Thread_Enable_dispatch();
  
  return RTEMS_SUCCESSFUL;
}

/*****************************************************
 * * get  the size of the specific mapped ALUT entry                        *
 * *****************************************************/
rtems_status_code rtems_memory_management_get_size(
  rtems_memory_management_entry* const mpe,
  size_t * size)
{
  rtems_mprot_alut* alut_p;
  alut_p = &the_rtems_mprot_alut;

  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();
  if(0 == mpe->region.bounds )
  {
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  *size = mpe->region.bounds;
  _Thread_Enable_dispatch();
  
  return RTEMS_SUCCESSFUL;
}


uint32_t
rtems_memory_management_get_default_permissions( void )
{
  return the_rtems_mprot_alut.rtems_mprot_default_permissions;
}

rtems_status_code rtems_memory_management_set_default_permissions(
  const uint32_t new_permissions,
  uint32_t* old_permissions )
{
  *old_permissions = the_rtems_mprot_alut.rtems_mprot_default_permissions;
  the_rtems_mprot_alut.rtems_mprot_default_permissions = new_permissions;

  /*Once the default attribute is changed, all of the related cache,tlb,and page table *
 *    * need to be updated. But because the ALUT is unsorted, it will spend much time to *
 *       * check whether each page is mapped in ALUT. So here we just flush the entire cache *
 *          * and initiliaze th pagetable and tlb*/
  _Thread_Disable_dispatch();
  rtems_pagetable_initialize();
  _Thread_Enable_dispatch();
  
  return RTEMS_SUCCESSFUL;
}
