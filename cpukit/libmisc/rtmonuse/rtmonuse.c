/*
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include <rtems/rtmonuse.h>

typedef struct {
  rtems_id     id;
  unsigned32   count;
  unsigned32   missed_count;
  unsigned32   min_cpu_time;
  unsigned32   max_cpu_time;
  unsigned32   total_cpu_time;
  unsigned32   min_wall_time;
  unsigned32   max_wall_time;
  unsigned32   total_wall_time;
}  Period_usage_t;

Period_usage_t *Period_usage_Information;

/*PAGE
 *
 *  Period_usage_Initialize
 */

void Period_usage_Initialize( void )
{
  int             maximum;

  maximum = _Configuration_Table->RTEMS_api_configuration->maximum_periods;
 
  Period_usage_Information = malloc( sizeof(Period_usage_t) * (maximum+1) );

  Period_usage_Reset();
}

/*PAGE
 *
 *  Period_usage_Reset
 */

void Period_usage_Reset( void )
{
  unsigned32      i;
  Period_usage_t *the_usage;

  for ( i=0 ;
        i<_Configuration_Table->RTEMS_api_configuration->maximum_periods ;
        i++ ) {
    the_usage = &Period_usage_Information[ i ];
 
    the_usage->count           = 0;
    the_usage->missed_count    = 0;
    the_usage->min_cpu_time    = 0xFFFFFFFF;
    the_usage->max_cpu_time    = 0;
    the_usage->total_cpu_time  = 0;
    the_usage->min_wall_time   = 0xFFFFFFFF;
    the_usage->max_wall_time   = 0;
    the_usage->total_wall_time = 0;
 
  }
}

/*PAGE
 *
 *  Period_usage_Update
 */

void Period_usage_Update( 
  rtems_id     id
)
{
  rtems_rate_monotonic_period_status rm_status;
  rtems_status_code                  status;
  Period_usage_t                    *the_usage;

  assert( Period_usage_Information );

  status = rtems_rate_monotonic_get_status( id, &rm_status );
  assert( status == RTEMS_SUCCESSFUL );

  the_usage = &Period_usage_Information[ rtems_get_index( id ) ];

  the_usage->id = id;
  the_usage->count++;
  if ( rm_status.state == RATE_MONOTONIC_EXPIRED )
    the_usage->missed_count++;
  the_usage->total_cpu_time  += rm_status.ticks_executed_since_last_period;
  the_usage->total_wall_time += rm_status.ticks_since_last_period;

  /*
   *  Update CPU time
   */

  if ( rm_status.ticks_executed_since_last_period < the_usage->min_cpu_time )
    the_usage->min_cpu_time = rm_status.ticks_executed_since_last_period;

  if ( rm_status.ticks_executed_since_last_period > the_usage->max_cpu_time )
    the_usage->max_cpu_time = rm_status.ticks_executed_since_last_period;

  /*
   *  Update Wall time
   */

  if ( rm_status.ticks_since_last_period < the_usage->min_wall_time )
    the_usage->min_wall_time = rm_status.ticks_since_last_period;

  if ( rm_status.ticks_since_last_period > the_usage->max_wall_time )
    the_usage->max_wall_time = rm_status.ticks_since_last_period;

}

/*PAGE
 *
 *  Period_usage_Dump
 */

void Period_usage_Dump( void )
{
  unsigned32              i;
  Period_usage_t         *the_usage;
  Rate_monotonic_Control *the_period;
  unsigned32              u32_name;
  char                   *cname;
  char                    name[5];
  unsigned32              api_index;
  Objects_Information    *information;
 
  if ( !Period_usage_Information ) {
    printf( "Period statistics library is not initialized\n" );
    return;
  }

  printf( "Period information by period\n" ); 
  printf( "   ID      OWNER   PERIODS  MISSED    CPU TIME    WALL TIME\n" );

  /*
   *  RTEMS does not use an index of zero for object ids.
   */

  for ( i=1 ;
        i<=_Configuration_Table->RTEMS_api_configuration->maximum_periods ;
        i++ ) {
    the_usage = &Period_usage_Information[ i ];
    if ( the_usage->count == 0 )
      continue;

    the_period =
      (Rate_monotonic_Control *)_Rate_monotonic_Information.local_table[ i ];

    name[ 0 ] = ' ';
    name[ 1 ] = ' ';
    name[ 2 ] = ' ';
    name[ 3 ] = ' ';
    name[ 4 ] = '\0';

    if ( the_period->owner ) {
      api_index = _Objects_Get_API(the_period->owner->Object.id);
      information = _Objects_Information_table[ api_index ][ 1 ];

      if ( information->is_string ) {
        cname = the_period->owner->Object.name;
        name[ 0 ] = cname[0];
        name[ 1 ] = cname[1];
        name[ 2 ] = cname[2];
        name[ 3 ] = cname[3];
        name[ 4 ] = '\0';
      } else {
        u32_name = (unsigned32)the_period->owner->Object.name;
        name[ 0 ] = (u32_name >> 24) & 0xff;
        name[ 1 ] = (u32_name >> 16) & 0xff;
        name[ 2 ] = (u32_name >>  8) & 0xff;
        name[ 3 ] = (u32_name >>  0) & 0xff;
        name[ 4 ] = '\0';
      }
    }

    if ( !isprint(name[0]) ) name[0] = '*';
    if ( !isprint(name[1]) ) name[1] = '*';
    if ( !isprint(name[2]) ) name[2] = '*';
    if ( !isprint(name[3]) ) name[3] = '*';


    printf(
      "0x%08x  %4s   %6d   %3d       %d/%d/%5.2f    %d/%d/%3.2f\n",
      the_usage->id,
      name,
      the_usage->count,
      the_usage->missed_count,
      the_usage->min_cpu_time,
      the_usage->max_cpu_time,
      (double) the_usage->total_cpu_time / (double) the_usage->count,
      the_usage->min_wall_time,
      the_usage->max_wall_time,
      (double) the_usage->total_wall_time / (double) the_usage->count
    );
  }
}
