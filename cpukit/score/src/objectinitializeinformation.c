/*
 *  Object Handler Initialization per Object Class
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*
 *  _Objects_Initialize_information
 *
 *  This routine initializes all object information related data structures.
 *
 *  Input parameters:
 *    information         - object information table
 *    maximum             - maximum objects of this class
 *    size                - size of this object's control block
 *    is_string           - true if names for this object are strings
 *    maximum_name_length - maximum length of each object's name
 *    When multiprocessing is configured,
 *      supports_global     - true if this is a global object class
 *      extract_callout     - pointer to threadq extract callout
 *
 *  Output parameters:  NONE
 */

void _Objects_Initialize_information(
  Objects_Information *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  uint16_t             size,
  bool                 is_string,
  uint32_t             maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
  ,
  bool                 supports_global,
  Objects_Thread_queue_Extract_callout extract
#endif
)
{
  static Objects_Control *null_local_table = NULL;
  uint32_t                minimum_index;
  Objects_Maximum         maximum_per_allocation;
  #if defined(RTEMS_MULTIPROCESSING)
    uint32_t              index;
  #endif

  information->the_api            = the_api;
  information->the_class          = the_class;
  information->size               = size;
  information->local_table        = 0;
  information->inactive_per_block = 0;
  information->object_blocks      = 0;
  information->inactive           = 0;
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    information->is_string        = is_string;
  #endif

  /*
   *  Set the maximum value to 0. It will be updated when objects are
   *  added to the inactive set from _Objects_Extend_information()
   */
  information->maximum = 0;

  /*
   *  Register this Object Class in the Object Information Table.
   */
  _Objects_Information_table[ the_api ][ the_class ] = information;

  /*
   *  Are we operating in limited or unlimited (e.g. auto-extend) mode.
   */
  information->auto_extend = _Objects_Is_unlimited( maximum );
  maximum_per_allocation = _Objects_Maximum_per_allocation( maximum );

  /*
   *  Unlimited and maximum of zero is illogical.
   */
  if ( information->auto_extend && maximum_per_allocation == 0) {
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0
    );
  }

  /*
   *  The allocation unit is the maximum value
   */
  information->allocation_size = maximum_per_allocation;

  /*
   *  Provide a null local table entry for the case of any empty table.
   */
  information->local_table = &null_local_table;

  /*
   *  Calculate minimum and maximum Id's
   */
  minimum_index = (maximum_per_allocation == 0) ? 0 : 1;
  information->minimum_id =
    _Objects_Build_id( the_api, the_class, _Objects_Local_node, minimum_index );

  /*
   *  Calculate the maximum name length
   *
   *  NOTE: Either 4 bytes for Classic API names or an arbitrary
   *        number for POSIX names which are strings that may be
   *        an odd number of bytes.
   */

  information->name_length = maximum_name_length;

  _Chain_Initialize_empty( &information->Inactive );

  /*
   *  Initialize objects .. if there are any
   */
  if ( maximum_per_allocation ) {
    /*
     *  Always have the maximum size available so the current performance
     *  figures are create are met.  If the user moves past the maximum
     *  number then a performance hit is taken.
     */
    _Objects_Extend_information( information );
  }

  /*
   *  Take care of multiprocessing
   */
  #if defined(RTEMS_MULTIPROCESSING)
    information->extract = extract;

    if ( (supports_global == true) && _System_state_Is_multiprocessing ) {

      information->global_table =
        (Chain_Control *) _Workspace_Allocate_or_fatal_error(
          (_Objects_Maximum_nodes + 1) * sizeof(Chain_Control)
        );

      for ( index=1; index <= _Objects_Maximum_nodes ; index++ )
        _Chain_Initialize_empty( &information->global_table[ index ] );
     }
     else
       information->global_table = NULL;
  #endif
}
