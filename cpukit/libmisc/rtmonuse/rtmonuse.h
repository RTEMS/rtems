/**
 * @file rtems/rtmonuse.h
 */
 
/*
 *  $Id$
 */

#ifndef __RATE_MONOTONIC_USAGE_h
#define __RATE_MONOTONIC_USAGE_h

#ifdef __cplusplus
extern "C" {
#endif

void Period_usage_Initialize( void );

void Period_usage_Reset( void );

void Period_usage_Update(
  rtems_id     id
);

void Period_usage_Dump( void );

#ifdef __cplusplus
}
#endif

#endif
