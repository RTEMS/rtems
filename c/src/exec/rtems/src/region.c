/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/core/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>

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
  _Objects_Initialize_information(
    &_Region_Information,
    OBJECTS_RTEMS_REGIONS,
    FALSE,
    maximum_regions,
    sizeof( Region_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );

  /*
   *  Register the MP Process Packet routine.
   */

  _MPCI_Register_packet_processor(
    MP_PACKET_REGION,
    0  /* XXX _Region_MP_Process_packet */
  );

}

/*PAGE
 *
 *  rtems_region_create
 *
 *  This directive creates a region of physical contiguous memory area
 *  from which variable sized segments can be allocated.
 *
 *  Input parameters:
 *    name             - user defined region name
 *    starting_address - physical start address of region
 *    length           - physical length in bytes
 *    page_size        - page size in bytes
 *    attribute_set    - region attributes
 *    id               - address of region id to set
 *
 *  Output parameters:
 *    id       - region id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_create(
  rtems_name          name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          page_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
)
{
  Region_Control *the_region;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !_Addresses_Is_aligned( starting_address ) )
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();             /* to prevent deletion */

  the_region = _Region_Allocate();

  if ( !the_region ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_region->maximum_segment_size =
    _Heap_Initialize(&the_region->Memory, starting_address, length, page_size);

  if ( !the_region->maximum_segment_size ) {
    _Region_Free( the_region );
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_SIZE;
  }

  the_region->starting_address      = starting_address;
  the_region->length                = length;
  the_region->page_size             = page_size;
  the_region->attribute_set         = attribute_set;
  the_region->number_of_used_blocks = 0;

  _Thread_queue_Initialize(
    &the_region->Wait_queue,
    OBJECTS_RTEMS_REGIONS,
    _Attributes_Is_priority( attribute_set ) ? 
       THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SEGMENT,
    _Region_MP_Send_extract_proxy,
    RTEMS_TIMEOUT
  );

  _Objects_Open( &_Region_Information, &the_region->Object, &name );

  *id = the_region->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_region_ident
 *
 *  This directive returns the system ID associated with
 *  the region name.
 *
 *  Input parameters:
 *    name - user defined region name
 *    id   - pointer to region id
 *
 *  Output parameters:
 *    *id      - region id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Region_Information,
    &name,
    OBJECTS_SEARCH_LOCAL_NODE,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}

/*PAGE
 *
 *  rtems_region_delete
 *
 *  This directive allows a thread to delete a region specified by
 *  the region identifier, provided that none of its segments are
 *  still allocated.
 *
 *  Input parameters:
 *    id - region id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_delete(
  Objects_Id id
)
{
  register Region_Control *the_region;
  Objects_Locations               location;

  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:
      _Region_Debug_Walk( the_region, 5 );
      if ( the_region->number_of_used_blocks == 0 ) {
        _Objects_Close( &_Region_Information, &the_region->Object );
        _Region_Free( the_region );
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_RESOURCE_IN_USE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_region_extend
 *
 *  This directive attempts to grow a region of physical contiguous memory area
 *  from which variable sized segments can be allocated.
 *
 *  Input parameters:
 *    id         - id of region to grow
 *    start      - starting address of memory area for extension
 *    length     - physical length in bytes to grow the region
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_extend(
  Objects_Id          id,
  void               *starting_address,
  unsigned32          length
)
{
  Region_Control     *the_region;
  Objects_Locations   location;
  unsigned32          amount_extended;
  Heap_Extend_status  heap_status;
  rtems_status_code   status;

  status = RTEMS_SUCCESSFUL;

  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:

      heap_status = _Heap_Extend(
        &the_region->Memory,
        starting_address,
        length,
        &amount_extended
      );

      switch ( heap_status ) {
        case HEAP_EXTEND_SUCCESSFUL:
          the_region->length                += amount_extended;
          the_region->maximum_segment_size  += amount_extended;
          break;
        case HEAP_EXTEND_ERROR:
          status = RTEMS_INVALID_ADDRESS;
          break;
        case HEAP_EXTEND_NOT_IMPLEMENTED:
          status = RTEMS_NOT_IMPLEMENTED;
          break;
      }
      _Thread_Enable_dispatch();
      return( status );
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_region_get_segment
 *
 *  This directive will obtain a segment from the given region.
 *
 *  Input parameters:
 *    id         - region id
 *    size       - segment size in bytes
 *    option_set - wait option
 *    timeout    - number of ticks to wait (0 means wait forever)
 *    segment    - pointer to segment address
 *
 *  Output parameters:
 *    segment    - pointer to segment address filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_get_segment(
  Objects_Id         id,
  unsigned32         size,
  rtems_option       option_set,
  rtems_interval     timeout,
  void              **segment
)
{
  register Region_Control *the_region;
  Objects_Locations        location;
  Thread_Control          *executing;
  void                    *the_segment;

  if ( size == 0 )
    return RTEMS_INVALID_SIZE;

  executing  = _Thread_Executing;
  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:
      if ( size > the_region->maximum_segment_size ) {
        _Thread_Enable_dispatch();
        return RTEMS_INVALID_SIZE;
      }

      _Region_Debug_Walk( the_region, 1 );

      the_segment = _Region_Allocate_segment( the_region, size );

      _Region_Debug_Walk( the_region, 2 );

      if ( the_segment ) {
        the_region->number_of_used_blocks += 1;
        _Thread_Enable_dispatch();
        *segment = the_segment;
        return RTEMS_SUCCESSFUL;
      }

      if ( _Options_Is_no_wait( option_set ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_UNSATISFIED;
      }

      executing->Wait.queue           = &the_region->Wait_queue;
      executing->Wait.id              = id;
      executing->Wait.count           = size;
      executing->Wait.return_argument = (unsigned32 *) segment;

      the_region->Wait_queue.sync = TRUE;

      _Thread_queue_Enqueue( &the_region->Wait_queue, timeout );

      _Thread_Enable_dispatch();
      return( executing->Wait.return_code );
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
/*PAGE
 *
 *  rtems_region_get_segment_size
 *
 *  This directive will return the size of the segment indicated
 *
 *  Input parameters:
 *    id         - region id
 *    segment    - segment address
 *    size       - pointer to segment size in bytes
 *
 *  Output parameters:
 *    size       - segment size in bytes filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_get_segment_size(
  Objects_Id         id,
  void              *segment,
  unsigned32        *size
)
{
  register Region_Control *the_region;
  Objects_Locations        location;
  Thread_Control          *executing;

  executing  = _Thread_Executing;
  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:

      if ( _Heap_Size_of_user_area( &the_region->Memory, segment, size ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_region_return_segment
 *
 *  This directive will return a segment to its region.
 *
 *  Input parameters:
 *    id      - region id
 *    segment - pointer to segment address
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_region_return_segment(
  Objects_Id  id,
  void       *segment
)
{
  register Region_Control *the_region;
  Thread_Control          *the_thread;
  Objects_Locations        location;
  void                   **the_segment;
  int                      status;

  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:

      _Region_Debug_Walk( the_region, 3 );

      status = _Region_Free_segment( the_region, segment );

      _Region_Debug_Walk( the_region, 4 );

      if ( !status ) {
        _Thread_Enable_dispatch();
        return RTEMS_INVALID_ADDRESS;
      }

      the_region->number_of_used_blocks -= 1;
      for ( ; ; ) {
        the_thread = _Thread_queue_First( &the_region->Wait_queue );

        if ( the_thread == NULL )
           break;

        the_segment = _Region_Allocate_segment(
           the_region, 
           the_thread->Wait.count
        );

        if ( the_segment == NULL )
           break;

        *(void **)the_thread->Wait.return_argument = the_segment;
        the_region->number_of_used_blocks += 1;
        _Thread_queue_Extract( &the_region->Wait_queue, the_thread );
        the_thread->Wait.return_code = RTEMS_SUCCESSFUL;
      }

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
