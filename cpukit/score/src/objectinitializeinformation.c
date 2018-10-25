/**
 * @file
 *
 * @brief Initialize Object Information
 * @ingroup ScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/wkspace.h>

void _Objects_Do_initialize_information(
  Objects_Information *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  uint16_t             size,
  bool                 is_string,
  uint32_t             maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Objects_Thread_queue_Extract_callout extract
#endif
)
{
  static Objects_Control *null_local_table = NULL;
  uint32_t                minimum_index;
  Objects_Maximum         maximum_per_allocation;

  information->the_api            = the_api;
  information->the_class          = the_class;
  information->size               = size;
  information->local_table        = 0;
  information->inactive_per_block = 0;
  information->object_blocks      = 0;
  information->inactive           = 0;
  information->is_string          = is_string;

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
    _Internal_error( INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0 );
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
    _RBTree_Initialize_empty( &information->Global_by_id );
    _RBTree_Initialize_empty( &information->Global_by_name );
  #endif
}
