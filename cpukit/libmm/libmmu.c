/**
 * Copyright (C) 2011. Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libmmu.h>
#include <rtems/score/isr.h>

/**
 *  Initialize the protection domain the_domain with preallocated
 *  array of max_number_of_mpes memory protection entries.
 */
rtems_status_code rtems_mm_initialize_domain(
    rtems_mm_domain* the_domain,
    size_t max_number_of_mpes
) {
  int i;
  rtems_mm_region_descriptor *the_region;
  rtems_mm_entry             *the_mpe;
  ISR_Level                      level;

  //_ISR_Disable( level );

  // TODO: configure space from workspace
  the_domain->mpe_array = _Workspace_Allocate(
      max_number_of_mpes * sizeof(rtems_mm_entry)
  );
  if ( ! the_domain->mpe_array ) {
    return RTEMS_NO_MEMORY;
  }

  // TODO: make this configurable? Put a bound on it?
  the_domain->mpe_array_size = max_number_of_mpes;

  _Chain_Initialize(
      &(the_domain->idle_mpe_chain),
      the_domain->mpe_array,
      max_number_of_mpes,
      sizeof(rtems_mm_entry)
  );

  _Chain_Initialize_empty( &(the_domain->active_mpe_chain) );
  
  for ( i = 0;i < max_number_of_mpes; i++ )
  {
    the_mpe = &the_domain->mpe_array[i];
    the_region = &the_mpe->region;
    the_region->name      = NULL;
    the_region->base      = NULL;
    the_region->bounds    = 0;
    the_mpe->installed    = false;
    the_mpe->permissions  = RTEMS_MEMORY_PROTECTION_NO_PERMISSION;
  }
 
  //_ISR_Enable( level );

  return RTEMS_SUCCESSFUL;
}

/**
 *  Release any resources associated with the_domain
 */
rtems_status_code rtems_mm_finalize_domain(
    rtems_mm_domain* the_domain
) {
  ISR_Level                      level;

 //_ISR_Disable( level );

  if ( 0 == the_domain ) {
    _ISR_Enable( level );
    return RTEMS_INVALID_ADDRESS;
  }

  rtems_mm_uninstall_domain(the_domain);
  if (the_domain->mpe_array)
    _Workspace_Free(the_domain->mpe_array);

  //_ISR_Enable( level );

  return RTEMS_SUCCESSFUL;
}

/**
 * Install all mpes in the protection domain the_domain.
 */
rtems_status_code rtems_mm_install_domain(
    rtems_mm_domain* the_domain
) {
  rtems_mm_entry *mpe;
  rtems_chain_node              *node;
  rtems_status_code              status;
  ISR_Level          level;

  //_ISR_Disable( level );

  status = RTEMS_SUCCESSFUL;

  node = rtems_chain_first( &the_domain->active_mpe_chain );
  while ( ! rtems_chain_is_tail( &the_domain->active_mpe_chain, node ) ) {
    mpe = (rtems_mm_entry*)node;
    
    if ( ! mpe->installed ) {
      status = rtems_mm_install_entry( mpe );
      if ( status != RTEMS_SUCCESSFUL ) {
        goto out;
      }
    }

    node = rtems_chain_next( node );
  }

out:
  //_ISR_Enable( level );
  return status;
}

/**
 *  Uninstall all mpes within the protection domain the_domain.
 *  FIXME: for CPUs that cannot do individual MPE uninstall this 
 *  function should just make one call to uninstall_entry?
 */
rtems_status_code rtems_mm_uninstall_domain(
    rtems_mm_domain* the_domain
) {
  rtems_mm_entry *mpe;
  rtems_chain_node              *node;
  rtems_status_code              status;
  ISR_Level          level;

  //_ISR_Disable( level );

  status = RTEMS_SUCCESSFUL;

  node = rtems_chain_first( &the_domain->active_mpe_chain );
  while ( ! rtems_chain_is_tail( &the_domain->active_mpe_chain, node ) ) {
    mpe = (rtems_mm_entry*)node;

    if ( mpe->installed ) {
      status = rtems_mm_uninstall_entry( mpe );
      if ( status != RTEMS_SUCCESSFUL )
        goto out;
    }
    
    node = rtems_chain_next( node );
  }

out:
  //_ISR_Enable( level );
  return status;
}

/**
 * Create a new memory protection entry for the region with
 * permissions in perms. This function adds the newly created mpe
 * to the active chain of the domain but does not install it. 
 */
rtems_status_code rtems_mm_create_entry(
  rtems_mm_domain* the_domain,
  rtems_mm_region_descriptor * const region,
  const rtems_mm_permission perms,
  rtems_mm_entry** p_ret)
{ 
  rtems_mm_entry    *current;
  rtems_chain_node                 *node;
  rtems_status_code                 status;
  void* const                       base = region->base;
  const size_t                      size = region->bounds;
  ISR_Level             level;

  //_ISR_Disable( level );

  status = RTEMS_SUCCESSFUL;

  if ( NULL == p_ret ) {
    status = RTEMS_INVALID_ADDRESS;
    goto out;
  }
  *p_ret = NULL;

  /* Check for invalid block size */
  status = rtems_mm_verify_size(size);
  if( status != RTEMS_SUCCESSFUL )
    goto out;

  /*advanced permission check , need to make sure the permission of the *
  * new block is available. the check is a CPU related function.*/
  status = rtems_mm_verify_permission(perms);
  if( status != RTEMS_SUCCESSFUL)
    goto out;

  /* Check for full condition */
  if ( rtems_chain_is_empty(&(the_domain->idle_mpe_chain)) ) {
    status = RTEMS_TOO_MANY;
    goto out;
  }

  /* Check for address map overlaps */
  node = rtems_chain_first( &the_domain->active_mpe_chain );
  while ( ! rtems_chain_is_tail(&the_domain->active_mpe_chain, node) ) {
    current = (rtems_mm_entry*)node;
    if ( !((current->region.base >= ( base + size) ) ||
          ((current->region.base + current->region.bounds ) <= base) )
    ) {
      status = RTEMS_INVALID_ADDRESS;
      goto out;
    }
    node = rtems_chain_next(&current->node);
  }
  
  /* get an empty node */
  node = rtems_chain_get_unprotected(&(the_domain->idle_mpe_chain));
  current = (rtems_mm_entry*)node;

  /* Append entry to the active chain */
  current->region.base = base;
  current->region.bounds = size;
  current->permissions = perms;
  rtems_chain_append_unprotected(&the_domain->active_mpe_chain, node);
  
  *p_ret = current;

out:
  _ISR_Enable( level );
  return status;
}

/**
 * Delete memory protection entry mpe from the protection
 * domain the_domain. 
 */
rtems_status_code rtems_mm_delete_entry(
  rtems_mm_domain* the_domain,
  rtems_mm_entry* const mpe
) {
  rtems_mm_entry *current;
  rtems_chain_node              *node;
  rtems_status_code              status;
  ISR_Level          level;

  //_ISR_Disable( level );
  status = RTEMS_SUCCESSFUL;

  if ( 0 == mpe ) {
    status = RTEMS_INVALID_ADDRESS;
    goto out;
  }

  rtems_chain_extract_unprotected( &mpe->node );

  if ( mpe->installed )
    status = rtems_mm_uninstall_entry(mpe);

  mpe->region.bounds = 0;
  mpe->region.base = 0;
  rtems_chain_append_unprotected(&the_domain->idle_mpe_chain, &mpe->node);

out:
  _ISR_Enable( level );
  return status;
}

/**
 * Find the memory protection entry in the_domain that contains
 * the_address.
 */
rtems_status_code rtems_mm_find_entry(
  rtems_mm_domain* the_domain,
  void* const the_address 
  )
{
  rtems_mm_entry* current;
  rtems_chain_node *node;
  rtems_status_code              status;
  ISR_Level          level;

  //_ISR_Disable( level );
  status = RTEMS_SUCCESSFUL;

  /*if( 0 == p_ret ) {
    status = RTEMS_INVALID_ADDRESS;
    goto out;
  }
  *p_ret = 0; */

  node = rtems_chain_first( &the_domain->active_mpe_chain );

  while ( ! rtems_chain_is_tail( &(the_domain->active_mpe_chain), node ) ) {
    current = (rtems_mm_entry*)node;
    if ( (current->region.base <= the_address) &&
         ((current->region.base + current->region.bounds)>the_address) 
       ) {
     // *p_ret = current;
      goto out;
    }
    node = rtems_chain_next(node);
  }

  status = RTEMS_UNSATISFIED;

out:
  //_ISR_Enable( level );
  return status;
}

/**
 * Set the permissions of mpe to new_permission. This function does
 * not change whether mpe is installed or not: if the mpe is installed then
 * it will be re-installed with the new permissions.
 */
rtems_status_code rtems_mm_set_permission(
  rtems_mm_entry* const mpe,
  const rtems_mm_permission new_permission,
  rtems_mm_permission* old_permission )
{
  rtems_status_code              status;
  ISR_Level          level;

  //_ISR_Disable( level );
  status = RTEMS_SUCCESSFUL;

  if ( 0 == mpe ) {
    status = RTEMS_INVALID_ADDRESS;
    goto out;
  }

  if( 0 == mpe->region.bounds ) {
    status = RTEMS_INVALID_ADDRESS;
    goto out;
  }

  /* advanced permission check to make sure the permission of the
   * new block is available. the check is a CPU related function.
   */
  status = rtems_mm_verify_permission(new_permission);
  if( status != RTEMS_SUCCESSFUL)
    goto out;

  *old_permission = mpe->permissions;
  if( *old_permission == new_permission) {
    goto out; /* no work to do */
  }

  mpe->permissions = new_permission;
  if ( mpe->installed ) {
    // FIXME: on some architecture it might be better to replace an entry.
    // Not sure how often this would happen though. and some architectures
    // might not even allow fine-grained removal / installation of entries.
    rtems_mm_uninstall_entry(mpe);
    rtems_mm_install_entry(mpe);
  }

out: 
  //_ISR_Enable( level );
  return status;
}
