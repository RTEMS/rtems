/*
 * @file libmm.h
 *
 * @ingroup libmm
 *
 * @brief libmm API make use of MPU/MMU units to provide 
 * memory management.
 */

/* COPYRIGHT (c) 1989-2011.
 *
 * On-Line Applications Research Corporation (OAR).
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

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
#include "libmm.h"


/* TODO: rename ALUT to Arena */

static rtems_memory_management_alut the_rtems_memory_management_alut;

rtems_status_code rtems_memory_management_install_alut(void) {
    
  rtems_memory_management_alut* the_alut= &the_rtems_memory_management_alut;
  int i;

  _Chain_Initialize(&(the_alut->ALUT_idle), the_alut->entries,
    RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE, sizeof(rtems_memory_management_entry) );

  _Chain_Initialize_empty(&(the_alut->ALUT_mappings));

  for( i = 0; i < RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE; i++)
  {
    the_alut->entries[i].region.size = 0;
    the_alut->entries[i].region.base = (void*)0;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_memory_management_find_entry(
  void* const addr, 
  rtems_memory_management_entry** p_ret)
{
  rtems_memory_management_alut* alut_p;
  rtems_memory_management_entry* current;
  alut_p = &the_rtems_memory_management_alut;

  if( p_ret == 0 )   
    return RTEMS_INVALID_ADDRESS;

  current = (rtems_memory_management_entry* )\
  rtems_chain_first( &alut_p->ALUT_mappings);
  while(!rtems_chain_is_tail(&(alut_p->ALUT_mappings),&current->node)){
    if( (current->region.base <= addr ) && \
      ((current->region.base+current->region.size )>addr) ){
      *p_ret = current;
      return RTEMS_SUCCESSFUL;
    }
    current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  return RTEMS_UNSATISFIED;
}

rtems_status_code rtems_memory_management_create_entry(
  rtems_memory_management_region_descriptor region,
  rtems_memory_management_entry** p_ret)
{
  rtems_memory_management_entry* current;
  rtems_memory_management_alut* alut_p;
  rtems_status_code status;
  alut_p = &the_rtems_memory_management_alut;

  /* Check for invalid block size */
  status = rtems_memory_management_verify_size(region.size);
  if( status != RTEMS_SUCCESSFUL )
    return RTEMS_INVALID_NUMBER;

  _Thread_Disable_dispatch();
  /* Check for address map overlaps */
  current = (rtems_memory_management_entry* )rtems_chain_first(&alut_p->ALUT_mappings);
  while( !rtems_chain_is_tail(&alut_p->ALUT_mappings,&current->node)){
    if( !((current->region.base >= ( region.base + region.size) ) ||\
      ((current->region.base +current->region.size ) <= region.base) )){
        _Thread_Enable_dispatch();
        return RTEMS_INVALID_ADDRESS;
      }
    current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  
   /* Check for ALUT full condition and get a valid empty entry */
  if( rtems_chain_is_empty(&(alut_p->ALUT_idle)) )
  {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
  
  current = (rtems_memory_management_entry* )rtems_chain_get_unprotected(&(alut_p->ALUT_idle));
  
 /* Append entry to the ALUT */
  current->region.base = region.base;
  current->region.size = region.size;
  rtems_chain_append_unprotected(&alut_p->ALUT_mappings, &current->node);
  
  /* for the new entry block , the attribute may be different from the previous value*
   *  so update the related cache tlb and pagetable entry*/

  _Thread_Enable_dispatch();
  
  *p_ret = current;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_memory_management_delete_entry(
  rtems_memory_management_entry* const mpe)
{
  rtems_memory_management_alut    *alut_p;
  rtems_memory_management_entry   *current;
  alut_p = &the_rtems_memory_management_alut;

  _Thread_Disable_dispatch();
  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  /*make sure the mpe point to a mapped entry */
  current = (rtems_memory_management_entry* )rtems_chain_first(&alut_p->ALUT_mappings);
  while(!rtems_chain_is_tail(&alut_p->ALUT_mappings, &current->node)){
    if( rtems_chain_are_nodes_equal(&current->node,&mpe->node )){

      rtems_chain_extract_unprotected(&mpe->node );

      /* for the delete entry block , the attribute should be changed default access attribute*
       *  so  update the related cache tlb and pagetable entry*/
      
      mpe->region.size = 0;
      mpe->region.base = 0;
      rtems_chain_append_unprotected(&alut_p->ALUT_idle, &current->node);

      _Thread_Enable_dispatch();  
      return RTEMS_SUCCESSFUL;
    }else
      current = (rtems_memory_management_entry* )rtems_chain_next(&current->node);
  }
  
   _Thread_Enable_dispatch();
  return RTEMS_INVALID_ADDRESS;
}

rtems_status_code rtems_memory_management_get_size(
  rtems_memory_management_entry* const mpe,
  size_t * size)
{
  rtems_memory_management_alut* alut_p;
  alut_p = &the_rtems_memory_management_alut;

  if( 0 == mpe || mpe < alut_p->entries ||\
    mpe > (alut_p->entries+ sizeof(alut_p->entries) ))
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();
  if(0 == mpe->region.size )
  {
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  *size = mpe->region.size;
  _Thread_Enable_dispatch();
  
  return RTEMS_SUCCESSFUL;
}

#endif
