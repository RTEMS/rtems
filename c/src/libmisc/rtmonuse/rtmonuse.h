/*
 *  $Id$
 */

#ifndef __RATE_MONOTONIC_USAGE_h
#define __RATE_MONOTONIC_USAGE_h

void Period_usage_Initialize( void );

void Period_usage_Reset( void );

void Period_usage_Update( 
  rtems_id     id
);

void Period_usage_Dump( void );

#endif
