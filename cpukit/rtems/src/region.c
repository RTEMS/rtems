/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/apimutex.h>

/*PAGE
 *
 *  _Region_Manager_initialization
 *
 *  This routine initializes all region manager related data structures.
 *
 *  Input parameters:
 *    maximum_regions - number of regions to initialize
 *
 *  Output parameters:  NONE
 */

void _Region_Manager_initialization(
  unsigned32 maximum_regions
)
{
  /* XXX move me */
  _API_Mutex_Initialization( 1 );
  _API_Mutex_Allocate( _RTEMS_Allocator_Mutex );

  _Objects_Initialize_information(
    &_Region_Information,      /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_REGIONS,     /* object class */
    maximum_regions,           /* maximum objects of this class */
    sizeof( Region_Control ),  /* size of this object's control block */
    FALSE,                     /* TRUE if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                     /* TRUE if this is a global object class */
    NULL                       /* Proxy extraction support callout */
#endif
  );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_REGION,
    0  /* XXX _Region_MP_Process_packet */
  );
#endif

}

